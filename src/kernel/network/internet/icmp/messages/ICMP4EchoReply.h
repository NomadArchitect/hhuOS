//
// Created by hannes on 17.05.21.
//

#ifndef HHUOS_ICMP4ECHOREPLY_H
#define HHUOS_ICMP4ECHOREPLY_H


#include <kernel/network/internet/IP4DataPart.h>
#include <kernel/network/internet/icmp/ICMP4Message.h>

class ICMP4EchoReply : public ICMP4Message{
public:
    ICMP4EchoReply();

    ICMP4EchoReply(IP4DataPart *dataPart);

    void *getMemoryAddress() override;

    uint16_t getLength() override;
};


#endif //HHUOS_ICMP4ECHOREPLY_H
