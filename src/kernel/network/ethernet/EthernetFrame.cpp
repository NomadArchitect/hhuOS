//
// Created by hannes on 15.05.21.
//

#include "EthernetFrame.h"

EthernetFrame::EthernetFrame(EthernetAddress *destinationAddress, uint16_t protocolType, NetworkDataPart *dataPart)
        : destinationAddress(destinationAddress), protocolType(protocolType), dataPart(dataPart) {}

void *EthernetFrame::getDataAsByteBlock() {
    return nullptr;
}

uint16_t EthernetFrame::getLength() {
    return 0;
}
