//
// Created by hannes on 14.05.21.
//

#include <kernel/network/internet/icmp/messages/ICMP4Echo.h>
#include <kernel/event/network/ICMP4SendEvent.h>
#include <cstdio>
#include <kernel/network/NetworkDefinitions.h>
#include "Ping.h"

Ping::Ping(Shell &shell) : Command(shell) {
    eventBus = new Kernel::NetworkEventBus(Kernel::System::getService<Kernel::EventBus>());
    timeService = Kernel::System::getService<Kernel::TimeService>();
}

void Ping::execute(Util::Array<String> &args) {
    Util::ArgumentParser parser(getHelpText(), 1);

    parser.addParameter("target","t", false);
    parser.addParameter("count", "n", false);

    if (!parser.parse(args)) {
        stderr << args[0] << ": " << parser.getErrorString() << endl;
        return;
    }

    uint8_t addressBytes[IP4ADDRESS_LENGTH]{127,0,0,1}, numberOfPings = 10;

    if(!parser.getNamedArgument("target").isEmpty()) {
        auto target = parser.getNamedArgument("target");

        auto addressParts = target.split(".");
        if (addressParts.length() != IP4ADDRESS_LENGTH) {
            stderr << "Malformed IPv4 address! We need exactly format [0-255].[0-255].[0-255].[0-255]" << endl;
            return;
        }

        for (uint8_t i = 0; i < IP4ADDRESS_LENGTH; i++) {
            addressBytes[i] = strtoint((const char *) addressParts[i]);
        }
    }

    if(!parser.getNamedArgument("number").isEmpty()){
        numberOfPings= strtoint((const char *) parser.getNamedArgument("number"));
    }

    for(uint8_t i=0;i<numberOfPings;i++){
        eventBus->publish(
                new Kernel::ICMP4SendEvent(
                        new IP4Address(addressBytes),
                        new ICMP4Echo(42,i)
                        )
        );
        timeService->msleep(1000);
    }


}

const String Ping::getHelpText() {
    return "Utility for testing our IP protocol stack via ICMP Echo and ICMP Echo Reply\n\n"
           "Usage: ping [OPTION]\n"
           "Options:\n"
           "   -h, --help: Show this help-message";
}