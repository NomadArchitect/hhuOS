//
// Created by hannes on 17.05.21.
//

#include <kernel/network/internet/addressing/IP4Address.h>
#include "ICMP4EchoReply.h"

ICMP4EchoReply::ICMP4EchoReply(uint16_t identifier, uint16_t sequenceNumber) {
    echoReply.type = 0; //8 for echo, 0 for echo reply (RFC792)
    echoReply.code = 0;
    echoReply.checksum = 0;
    echoReply.identifier = identifier;
    echoReply.sequenceNumber = sequenceNumber;
}

uint8_t ICMP4EchoReply::copyTo(NetworkByteBlock *output) {
    if (output == nullptr) {
        return 1;
    }

    uint8_t errors = 0;
    errors += output->append(echoReply.type);
    errors += output->append(echoReply.code);
    errors += output->append(echoReply.checksum);
    errors += output->append(echoReply.identifier);
    errors += output->append(echoReply.sequenceNumber);

    return errors;
}

size_t ICMP4EchoReply::getLengthInBytes() {
    return sizeof(echoReplyMessage);
}

uint16_t ICMP4EchoReply::getIdentifier() const {
    return echoReply.identifier;
}

uint16_t ICMP4EchoReply::getSequenceNumber() const {
    return echoReply.sequenceNumber;
}

ICMP4Message::ICMP4MessageType ICMP4EchoReply::getICMP4MessageType() {
    return ICMP4MessageType::ECHO_REPLY;
}

IP4Address *ICMP4EchoReply::getSourceAddress() const {
    return new IP4Address(ip4Info.sourceAddress);
}

void ICMP4EchoReply::setSourceAddress(IP4Address *sourceAddress) {
    sourceAddress->copyTo(ip4Info.sourceAddress);
}

uint8_t ICMP4EchoReply::parse(NetworkByteBlock *input) {
    if (input == nullptr ||
        input->bytesRemaining() <= sizeof echoReply
            ) {
        return 1;
    }
    //NOTE: The first Byte for 'type' is already read in IP4Datagram!
    //-> the next Byte in our NetworkByteBlock is 'code', the next value!
    //This is no problem here, because the 'type' value is constant '0' per definition
    uint8_t errors = 0;
    errors += input->read(&echoReply.code);
    errors += input->read(&echoReply.checksum);
    errors += input->read(&echoReply.identifier);
    errors += input->read(&echoReply.sequenceNumber);

    return errors;
}
