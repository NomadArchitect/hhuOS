//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMP4MODULE_H
#define HHUOS_ICMP4MODULE_H

#include <kernel/network/NetworkEventBus.h>
#include <kernel/log/Logger.h>
#include <kernel/network/internet/icmp/ICMP4Message.h>

namespace Kernel {
    class ICMP4Module : public Receiver {
    private:
        NetworkEventBus *eventBus;

        uint8_t processICMP4Message(IP4Header *ip4Header, NetworkByteBlock *input);

    public:

        explicit ICMP4Module(NetworkEventBus *eventBus);


        /**
         * A logger to provide information on the kernel log.
         */
        Logger &log = Logger::get("ICMP4Module");

/**
     * Inherited method from Receiver.
     * This method is meant to be overridden and
     * implemented by this class.
     */
        void onEvent(const Event &event) override;

        static void deleteMessageSpecific(ICMP4Message *message);
    };
}

#endif //HHUOS_ICMP4MODULE_H
