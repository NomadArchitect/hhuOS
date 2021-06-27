//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_ICMP4ECHO_H
#define HHUOS_ICMP4ECHO_H

#include <kernel/network/internet/icmp/ICMP4Message.h>
#include "ICMP4EchoReply.h"

class ICMP4Echo final : public ICMP4Message {
private:
    typedef struct icmp4echo {
        uint16_t identifier = 0;
        uint16_t sequenceNumber = 0;
    } echo_t;

    echo_t echoMessage;

public:
    //Sending constructor
    ICMP4Echo(uint16_t identifier, uint16_t sequenceNumber);

    ICMP4Echo() = default;

    ~ICMP4Echo() = default;

    uint8_t copyTo(Kernel::NetworkByteBlock *output) override;

    size_t getLengthInBytes() override;

    ICMP4MessageType getICMP4MessageType() override;

    uint8_t parse(Kernel::NetworkByteBlock *input);

    [[nodiscard]] ICMP4EchoReply *buildEchoReply() const;
};


#endif //HHUOS_ICMP4ECHO_H
