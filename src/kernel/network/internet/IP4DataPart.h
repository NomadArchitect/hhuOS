//
// Created by hannes on 14.05.21.
//

#ifndef HHUOS_NETWORKDATAPART_H
#define HHUOS_NETWORKDATAPART_H

#include <cinttypes>
#include "IP4ProtocolType.h"
#include "IP4HeaderChecksum.h"

class IP4DataPart {
public:
    virtual void *getMemoryAddress() = 0;

    virtual uint16_t getLengthInBytes() = 0;

    virtual IP4ProtocolType getIP4ProtocolType() = 0;
};


#endif //HHUOS_NETWORKDATAPART_H
