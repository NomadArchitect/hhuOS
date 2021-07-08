//
// Created by hannes on 15.05.21.
//

#include <kernel/network/NetworkDefinitions.h>
#include <kernel/event/network/EthernetSendEvent.h>
#include <kernel/service/TimeService.h>
#include "ARPModule.h"

namespace Kernel {
    //Private method!
    //Why private? We might delete given addresses at any time or in destructor at least
    //-> very dangerous if public, we MUST make sure that only new [Address]() is given here
    uint8_t ARPModule::addEntry(IP4Address *ip4Address, EthernetAddress *ethernetAddress) {
        if (arpTable == nullptr || accessLock == nullptr) {
            log.error("ARP table or access lock was null, not adding entry");
            return 1;
        }
        accessLock->acquire();
        for (ARPEntry *current:*arpTable) {
            //Update existing entry instead of creating a new one if address already known
            if (current->matches(ip4Address)) {
                delete current->getEthernetAddress();
                current->setEthernetAddress(ethernetAddress);
                accessLock->release();
                return 0;
            }
        }
        arpTable->add(new ARPEntry(ip4Address, ethernetAddress));
        accessLock->release();
        return 0;
    }

    //Private method!
    bool ARPModule::entryFound(EthernetAddress **ethernetAddress, IP4Address *receiverAddress) {
        if (arpTable == nullptr || accessLock == nullptr) {
            log.error("ARP table or accessLock was null, not resolving");
            return false;
        }
        accessLock->acquire();
        *ethernetAddress = nullptr;
        for (ARPEntry *current:*arpTable) {
            if (current->matches(receiverAddress)) {
                *ethernetAddress = current->getEthernetAddress();
                accessLock->release();
                return true;
            }
        }
        accessLock->release();
        return false;
    }

    ARPModule::ARPModule(EventBus *eventBus, EthernetDevice *outDevice) {
        this->eventBus = eventBus;
        this->outDevice = outDevice;

        arpTable = new Util::ArrayList<ARPEntry *>();
        timeService = System::getService<TimeService>();
        broadcastAddress = EthernetAddress::buildBroadcastAddress();

        accessLock = new Spinlock();
        accessLock->release();
    }

    ARPModule::~ARPModule() {
        if (arpTable != nullptr) {
            ARPEntry *toDelete;
            for (uint32_t i = 0; i < arpTable->size(); i++) {
                toDelete = arpTable->get(i);
                arpTable->remove(i);
                i--;
                delete toDelete;
            }
        }
        delete arpTable;
    }

    uint8_t ARPModule::resolveTo(EthernetAddress **ethernetAddress, IP4Address *targetProtocolAddress,
                                 IP4Address *senderProtocolAddress) {
        if (ethernetAddress == nullptr || targetProtocolAddress == nullptr || senderProtocolAddress == nullptr) {
            log.error("At least one parameter was null, not resolving");
            return 1;
        }
        if (arpTable == nullptr || timeService == nullptr) {
            log.error("ARP table or time service was null, not resolving");
            return 1;
        }
        if (entryFound(ethernetAddress, targetProtocolAddress)) {
            return 0;
        }

        log.info("No entry found for %s, sending ARP request", targetProtocolAddress->asChars());
        sendRequest(senderProtocolAddress, targetProtocolAddress);

        timeService->msleep(ARP_WAIT_TIME);

        if (entryFound(ethernetAddress, targetProtocolAddress)) {
            log.info("ARP reply for address %s arrived in the last %d milliseconds",
                     targetProtocolAddress->asChars(), ARP_WAIT_TIME);
            return 0;
        }
        log.error("No ARP reply arrived in time, no resolve for %s possible",
                  targetProtocolAddress->asChars()
        );
        return 1;
    }

    uint8_t ARPModule::sendRequest(IP4Address *senderProtocolAddress, IP4Address *targetProtocolAddress) {
        if (senderProtocolAddress == nullptr || targetProtocolAddress == nullptr) {
            log.error("senderProtocolAddress or targetProtocolAddress was null, returning");
            return 1;
        }

        //See RFC 826 page 3 for details
        auto *arpRequest = new ARPMessage(
                1, // 1 for Ethernet
                (uint16_t) EthernetDataPart::EtherType::IP4, // 0x0800 for IPv4
                ETH_ADDRESS_LENGTH,
                IP4ADDRESS_LENGTH,
                ARPMessage::OpCode::REQUEST
        );

        uint8_t hardwareAddress[ETH_ADDRESS_LENGTH];
        uint8_t protocolAddress[IP4ADDRESS_LENGTH];

        outDevice->copyAddressTo(hardwareAddress);
        arpRequest->setSenderHardwareAddress(hardwareAddress);

        senderProtocolAddress->copyTo(protocolAddress);
        arpRequest->setSenderProtocolAddress(protocolAddress);

        broadcastAddress->copyTo(hardwareAddress);
        arpRequest->setTargetHardwareAddress(hardwareAddress);

        targetProtocolAddress->copyTo(protocolAddress);
        arpRequest->setTargetProtocolAddress(protocolAddress);

        //Send request to EthernetModule for further processing
        auto ethernetSendARPRequestEvent =
                Util::SmartPointer<Event>(new EthernetSendEvent(outDevice, broadcastAddress, arpRequest));
        eventBus->publish(ethernetSendARPRequestEvent);

        //Broadcast address will be deleted after sending here, all other addresses are just copied to ARP request
        //-> no other addresses will be deleted here!
        return 0;
    }

    String ARPModule::asString() {
        accessLock->acquire();
        if (arpTable->isEmpty()) {
            accessLock->release();
            return "[empty]";
        }

        String tableContent = "\n    ";
        for (ARPEntry *current:*arpTable) {
            tableContent += current->asString();
            tableContent += ",\n    ";
        }
        accessLock->release();
        return tableContent;
    }

    uint8_t ARPModule::processIncoming(ARPMessage *message) {
        if (message == nullptr) {
            log.error("Incoming ARP message was null, ignoring");
            return 1;
        }
        uint8_t processErrors = 0;
        switch (message->getOpCode()) {
            case ARPMessage::OpCode::REQUEST: {
                //Use incoming requests as updates
                processErrors += addEntry(new IP4Address(message->getSenderProtocolAddress()),
                                          new EthernetAddress(message->getSenderHardwareAddress())
                );
                if (processErrors) {
                    log.error("Could not process ARP Request: ARP table update failed!");
                    //Message will be deleted in IP4Module after processing
                    //-> no 'delete message' here!
                    return processErrors;
                }

                uint8_t myAddressAsBytes[ETH_ADDRESS_LENGTH];
                outDevice->copyAddressTo(myAddressAsBytes);

                auto *reply = message->buildReply(myAddressAsBytes);
                auto *destinationAddress = new EthernetAddress(myAddressAsBytes);

                //send reply to EthernetModule for further processing
                auto ethernetSendARPReplyEvent =
                        Util::SmartPointer<Event>(new EthernetSendEvent(outDevice, destinationAddress, reply));
                eventBus->publish(ethernetSendARPReplyEvent);

                //Message will be deleted in IP4Module after processing
                //-> no 'delete message' here!
                break;
            }
            case ARPMessage::OpCode::REPLY: {
                auto *senderHardwareAddress = new EthernetAddress(message->getSenderHardwareAddress());
                auto *senderProtocolAddress = new IP4Address(message->getSenderProtocolAddress());

                auto *targetHardwareAddress = new EthernetAddress(message->getTargetHardwareAddress());
                auto *targetProtocolAddress = new IP4Address(message->getTargetProtocolAddress());

                processErrors += addEntry(senderProtocolAddress, senderHardwareAddress);
                if (processErrors) {
                    log.error(("Could not process ARP Response: ARP table update failed!"));
                    return processErrors;
                }

                //Learn own addresses if not broadcast
                if (!targetHardwareAddress->equals(broadcastAddress)) {
                    processErrors += addEntry(targetProtocolAddress, targetHardwareAddress);
                }
                if (processErrors) {
                    log.error(("Could not process ARP Response: ARP table update failed!"));
                    return processErrors;
                }
                //Message will be deleted in IP4Module after processing
                //-> no 'delete message' here!
                break;
            }
            default: {
                log.error("Invalid opCode in incoming ARP message, ignoring");
                //Message will be deleted in IP4Module after processing
                //-> no 'delete message' here!
                return 1;
            }
        }
        //Message will be deleted in IP4Module after processing
        //-> no 'delete message' here!
        return 0;
    }
}