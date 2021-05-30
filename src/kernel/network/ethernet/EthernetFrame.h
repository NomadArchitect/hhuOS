//
// Created by hannes on 15.05.21.
//

#ifndef HHUOS_ETHERNETFRAME_H
#define HHUOS_ETHERNETFRAME_H

#define ETHERNETDATAPART_MAX_LENGTH 1500
#define ETHERNETHEADER_MAX_LENGTH 14

#include "EthernetAddress.h"
#include "EthernetDataPart.h"

class EthernetFrame {
private:
    //Defined internally, should not be visible outside
    //Usage of IP4Datagram should only happen via given public methods
    //-> changing our header's internal representation is possible at any time then!
    typedef struct ethernetHeader {
        uint8_t destinationAddress[6]{0,0,0,0,0,0};
        uint8_t sourceAddress[6]{0,0,0,0,0,0};
        uint8_t etherType[2]{0,0};
    } ethHeader_t;

    ethHeader_t header;
    EthernetDataPart *ethernetDataPart;

    static size_t getHeaderSizeInBytes();
public:
    EthernetFrame(EthernetAddress *destinationAddress, EthernetDataPart *ethernetDataPart);

    EthernetFrame(void *packet, uint16_t length);

    EthernetDataPart::EtherType getEtherType() const;

    EthernetDataPart *getDataPart() const;

    void *getDataAsByteBlock();

    uint16_t getTotalLengthInBytes();

    void setSourceAddress(EthernetAddress *sourceAddress);
};


#endif //HHUOS_ETHERNETFRAME_H
