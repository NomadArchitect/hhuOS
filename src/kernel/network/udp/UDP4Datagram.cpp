//
// Created by hannes on 17.05.21.
//

#include <lib/libc/printf.h>
#include "UDP4Datagram.h"

UDP4Datagram::UDP4Datagram(uint16_t sourcePort, uint16_t destinationPort, NetworkByteBlock *dataBytes) {
    header.destinationPort = destinationPort;
    header.sourcePort = sourcePort;
    this->dataBytes = dataBytes;
    this->length = dataBytes->getLength();
}

uint8_t UDP4Datagram::copyTo(NetworkByteBlock *output) {
    if (
            dataBytes == nullptr ||
            output == nullptr ||
            length > (size_t) (UDP4DATAPART_MAX_LENGTH - sizeof header) ||
            sizeof header > UDP4HEADER_MAX_LENGTH
            ) {
        return 1;
    }

    uint8_t errors = 0;
    errors += output->append(header.sourcePort);
    errors += output->append(header.destinationPort);
    errors += output->append(header.length);
    errors += output->append(header.checksum);

    //True if errors>0
    if (errors) {
        return errors;
    }

    //Append dataBytes if no errors occurred yet
    output->append(dataBytes, length);
    return 0;
}

size_t UDP4Datagram::getLengthInBytes() {
    return (sizeof header) + length;
}

IP4DataPart::IP4ProtocolType UDP4Datagram::getIP4ProtocolType() {
    return IP4ProtocolType::UDP;
}

uint8_t UDP4Datagram::parseHeader(NetworkByteBlock *input) {
    if (input == nullptr || input->bytesRemaining() < sizeof this->header) {
        return 1;
    }
    uint8_t errors = 0;
    errors += input->read(&header.sourcePort);
    errors += input->read(&header.destinationPort);
    errors += input->read(&header.length);
    errors += input->read(&header.checksum);

    return errors;
}

UDP4Datagram::~UDP4Datagram() {
    //TODO: Rework cleanup, this here will kill sendBuffer after first outgoing datagram!
    delete dataBytes;
}
