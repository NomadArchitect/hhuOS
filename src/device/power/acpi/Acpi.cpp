/*
 * Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
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
 */

#include "Acpi.h"
#include "kernel/paging/MemoryLayout.h"

namespace Device {

const Acpi::Rsdp *Acpi::rsdp{};
const Acpi::SdtHeader **Acpi::tables{};
uint32_t Acpi::numTables{};

void Acpi::copyAcpiTables(uint8_t *destination) {
    auto destinationAddress = Util::Memory::Address<uint32_t>(destination);

    // Find RSDP and copy its address to determine whether the RSDP has been found
    auto *rsdp = findRsdp();
    destinationAddress.setByte(rsdp == nullptr ? 0 : 1);
    destinationAddress = destinationAddress.add(1);
    if (rsdp == nullptr) {
        return;
    }

    // Copy RSDP
    destinationAddress.copyRange(Util::Memory::Address<uint32_t>(rsdp), sizeof(Rsdp));
    rsdp = reinterpret_cast<Rsdp*>(destinationAddress.get());
    destinationAddress = destinationAddress.add(sizeof(Rsdp));

    // Check and copy RSDT
    auto *rsdt = reinterpret_cast<SdtHeader*>(rsdp->rsdtAddress);
    if (!checkSdt(rsdt)) {
        rsdp->rsdtAddress = 0;
        return;
    }

    auto originalRsdtAddress = rsdp->rsdtAddress;
    auto copiedRsdtAddress = destinationAddress.get();

    rsdp->rsdtAddress = Kernel::MemoryLayout::PHYSICAL_TO_VIRTUAL(copiedRsdtAddress);
    destinationAddress.copyRange(Util::Memory::Address<uint32_t>(rsdt), rsdt->length);
    destinationAddress = destinationAddress.add(rsdt->length);

    // Copy all tables, pointed to by the RSDT
    auto **originalEntries = reinterpret_cast<SdtHeader**>(originalRsdtAddress + sizeof(SdtHeader));
    auto *copiedEntries = reinterpret_cast<uint32_t*>(copiedRsdtAddress + sizeof(SdtHeader));
    uint32_t numEntries = (rsdt->length - sizeof(SdtHeader)) / sizeof(uint32_t);

    for (uint32_t i = 0; i < numEntries; i++) {
        auto *sdt = originalEntries[i];
        if (checkSdt(sdt)) {
            destinationAddress.copyRange(Util::Memory::Address<uint32_t>(sdt), sdt->length);
            copiedEntries[i] = Kernel::MemoryLayout::PHYSICAL_TO_VIRTUAL(destinationAddress.get());
            destinationAddress = destinationAddress.add(sdt->length);
        }
    }
}

Acpi::Rsdp* Acpi::findRsdp() {
    auto ebdaStartAddress = *reinterpret_cast<uint16_t*>(0x0000040e) << 4;
    auto rsdpAddress = searchRsdp(ebdaStartAddress, ebdaStartAddress + 1023);
    if (rsdpAddress != nullptr) {
        return reinterpret_cast<Rsdp*>(rsdpAddress);
    }

    return reinterpret_cast<Rsdp*>(searchRsdp(0x000e0000, 0x000fffff));
}

Acpi::Rsdp* Acpi::searchRsdp(uint32_t startAddress, uint32_t endAddress) {
    char signature[8] = {'R', 'S', 'D', ' ', 'P', 'T', 'R', ' '};
    auto signatureAddress = Util::Memory::Address<uint32_t>(signature);
    startAddress = Util::Memory::Address<uint32_t>(startAddress).alignUp(16).get();

    for (uint32_t i = startAddress; i <= endAddress - sizeof(signature); i += 16) {
        auto address = Util::Memory::Address<uint32_t>(i);
        if (address.compareRange(signatureAddress, sizeof(signature)) == 0) {
            if (checkRsdp(reinterpret_cast<Rsdp*>(i))) {
                return reinterpret_cast<Rsdp*>(i);
            }
        }
    }

    return nullptr;
}

bool Acpi::checkRsdp(Acpi::Rsdp *rsdp) {
    uint32_t sum = 0;
    for (uint32_t i = 0; i < sizeof(Rsdp); i++) {
        sum += reinterpret_cast<uint8_t*>(rsdp)[i];
    }

    return static_cast<uint8_t>(sum) == 0;
}

bool Acpi::checkSdt(Acpi::SdtHeader *sdtHeader) {
    uint32_t sum = 0;
    for (uint32_t i = 0; i < sdtHeader->length; i++) {
        sum += reinterpret_cast<uint8_t*>(sdtHeader)[i];
    }

    return static_cast<uint8_t>(sum) == 0;
}

void Acpi::initialize(const uint8_t *acpiAddress) {
    auto rsdpFound = (acpiAddress[0] == 1);
    if (rsdpFound) {
        rsdp = reinterpret_cast<const Device::Acpi::Rsdp*>(acpiAddress + 1);
        tables = reinterpret_cast<const SdtHeader**>(rsdp->rsdtAddress + sizeof(SdtHeader));
        numTables = (reinterpret_cast<SdtHeader*>(rsdp->rsdtAddress)->length - sizeof(SdtHeader)) / sizeof(uint32_t);
    } else {
        rsdp = nullptr;
        tables = nullptr;
        numTables = 0;
    }
}

bool Acpi::isAvailable() {
    return rsdp != nullptr;
}

const Acpi::Rsdp &Acpi::getRsdp() {
    return *rsdp;
}

bool Acpi::hasTable(const char *signature) {
    for (uint32_t i = 0; i < numTables; i++) {
        if (Util::Memory::Address<uint32_t>(tables[i]->signature).compareString(signature)) {
            return true;
        }
    }

    return false;
}

const Acpi::SdtHeader &Acpi::getTable(const char *signature) {
    for (uint32_t i = 0; i < numTables; i++) {
        if (Util::Memory::Address<uint32_t>(tables[i]->signature).compareString(signature)) {
            return *tables[i];
        }
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Acpi: Table not found!");
}

Util::Data::Array<Util::Memory::String> Acpi::getAvailableTables() {
    Util::Data::Array<Util::Memory::String> signatures(numTables);
    for (uint32_t i = 0; i < numTables; i++) {
        signatures[i] = Util::Memory::String(reinterpret_cast<const uint8_t*>(tables[i]->signature), sizeof(SdtHeader::signature));
    }

    return signatures;
}

}