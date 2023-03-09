/*
 * Copyright (C) 2018-2023 Heinrich-Heine-Universitaet Duesseldorf,
 * Institute of Computer Science, Department Operating Systems
 * Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 * The UDP/IP stack is based on a bachelor's thesis, written by Hannes Feil.
 * The original source code can be found here: https://github.com/hhuOS/hhuOS/tree/legacy/network
 */

#ifndef HHUOS_IP4MODULE_H
#define HHUOS_IP4MODULE_H

#include <cstdint>

#include "kernel/network/NetworkModule.h"
#include "lib/util/network/ip4/Ip4Header.h"
#include "Ip4RoutingModule.h"
#include "lib/util/collection/Array.h"
#include "lib/util/collection/ArrayList.h"
#include "lib/util/collection/Collection.h"
#include "lib/util/collection/Iterator.h"
#include "lib/util/base/String.h"
#include "kernel/network/ip4/Ip4Interface.h"
#include "lib/util/async/ReentrantSpinlock.h"

namespace Device {
namespace Network {
class NetworkDevice;
}  // namespace Network
}  // namespace Device
namespace Kernel {
class Logger;
}  // namespace Kernel
namespace Util {
namespace Network {
namespace Ip4 {
class Ip4Address;
class Ip4SubnetAddress;
}  // namespace Ip4
}  // namespace Network

namespace Io {
class ByteArrayInputStream;
class ByteArrayOutputStream;
}  // namespace Stream
}  // namespace Util

namespace Kernel::Network::Ip4 {

class Ip4Module : public NetworkModule {

public:
    /**
     * Default Constructor.
     */
    Ip4Module() = default;

    /**
     * Copy Constructor.
     */
    Ip4Module(const Ip4Module &other) = delete;

    /**
     * Assignment operator.
     */
    Ip4Module &operator=(const Ip4Module &other) = delete;

    /**
     * Destructor.
     */
    ~Ip4Module() = default;

    Util::Array<Ip4Interface> getInterfaces(const Util::String &deviceIdentifier);

    Util::Array<Ip4Interface> getTargetInterfaces(const Util::Network::Ip4::Ip4Address &address);

    bool registerInterface(const Util::Network::Ip4::Ip4SubnetAddress &address, Device::Network::NetworkDevice &device);

    bool removeInterface(const Util::Network::Ip4::Ip4SubnetAddress &address, const Util::String &deviceIdentifier);

    void readPacket(Util::Io::ByteArrayInputStream &stream, LayerInformation information, Device::Network::NetworkDevice &device) override;

    Ip4RoutingModule& getRoutingModule();

    static Ip4Interface writeHeader(Util::Io::ByteArrayOutputStream &stream, const Util::Network::Ip4::Ip4Address &sourceAddress, const Util::Network::Ip4::Ip4Address &destinationAddress, Util::Network::Ip4::Ip4Header::Protocol protocol, uint16_t payloadLength);

    static uint16_t calculateChecksum(const uint8_t *buffer, uint32_t offset, uint32_t length);

private:

    Ip4RoutingModule routingModule;
    Util::ArrayList<Ip4Interface> interfaces;
    Util::Async::ReentrantSpinlock lock;

    static Kernel::Logger log;
};

}

#endif
