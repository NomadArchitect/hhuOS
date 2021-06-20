//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_IP4ADDRESS_H
#define HHUOS_IP4ADDRESS_H

#include <cstdint>
#include <lib/string/String.h>

class IP4Address {
private:
    uint8_t *address;

public:
    IP4Address(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth);

    explicit IP4Address(const uint8_t *bytes);

    explicit IP4Address(IP4Address *other);

    String asString();

    char *asChars();

    bool equals(IP4Address *other);

    void copyTo(uint8_t *target);

    uint8_t calculateAND(IP4Address **ANDedAddress, const uint8_t *netmask);

    virtual ~IP4Address();

    static uint8_t parseTo(uint8_t *targetBytes, String *fromString);
};


#endif //HHUOS_IP4ADDRESS_H
