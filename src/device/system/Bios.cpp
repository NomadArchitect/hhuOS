/*
 * Copyright (C) 2018-2024 Heinrich-Heine-Universitaet Duesseldorf,
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


#include "kernel/memory/MemoryLayout.h"
#include "lib/util/base/Address.h"
#include "device/cpu/Cpu.h"
#include "lib/util/base/Exception.h"
#include "device/time/rtc/Cmos.h"
#include "kernel/service/InterruptService.h"
#include "Bios.h"
#include "kernel/service/MemoryService.h"
#include "kernel/memory/GlobalDescriptorTable.h"
#include "kernel/service/Service.h"
#include "kernel/multiboot/Multiboot.h"
#include "kernel/service/InformationService.h"
#include "lib/util/async/ReentrantSpinlock.h"
#include "lib/util/base/String.h"

extern "C" {
    void bios_call(Kernel::Thread::Context *stack, uint32_t entryPoint);
    void bios_call_16_start();
    void bios_call_16_end();
    void bios_call_16_interrupt();
}

namespace Device {

Kernel::InterruptDescriptorTable::Descriptor *Bios::biosIdtDescriptor = nullptr;
Kernel::GlobalDescriptorTable Bios::biosGdt;
Util::Async::ReentrantSpinlock Bios::lock;

uint16_t Bios::construct16BitRegister(uint8_t lowerValue, uint8_t higherValue) {
    return lowerValue | (higherValue << 8);
}

uint8_t Bios::get8BitRegister(uint16_t value, Bios::RegisterHalf half) {
    if (half == LOWER) {
        return value & 0x00ff;
    } else if (half == HIGHER) {
        return (value & 0xff00) >> 8;
    }

    Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "BIOS: Invalid register half!");
}

bool Bios::isAvailable() {
    auto &multiboot = Kernel::Service::getService<Kernel::InformationService>().getMultibootInformation();
    return multiboot.hasKernelOption("bios") && multiboot.getKernelOption("bios") == "true";
}

void Bios::initialize() {
    if (!isAvailable()) {
        Util::Exception::throwException(Util::Exception::UNSUPPORTED_OPERATION, "BIOS calls are disabled!");
    }

    // Setup special GDT, only used for BIOS calls
    biosGdt.addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0x9a, 0x0c)); // 32-bit kernel code segment
    biosGdt.addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0xffffffff, 0x92, 0x0c)); // 32-bit kernel data segment
    biosGdt.addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0x000fffff, 0x9a, 0x00)); // 16-bit BIOS code segment
    biosGdt.addSegment(Kernel::GlobalDescriptorTable::SegmentDescriptor(0x00000000, 0x000fffff, 0x92, 0x00)); // 16-bit BIOS data segment

    // Setup IDT descriptor for 16-bit code
    biosIdtDescriptor = reinterpret_cast<Kernel::InterruptDescriptorTable::Descriptor*>(Kernel::MemoryLayout::BIOS_CALL_IDT.toAddress().get());
    *biosIdtDescriptor = Kernel::InterruptDescriptorTable::Descriptor(nullptr, 256 / 2); // Real mode entries are only half as wide

    // Copy 16-bit code to lower memory
    const auto sourceAddress = Util::Address<uint32_t>(reinterpret_cast<uint32_t>(&bios_call_16_start));
    const auto targetAddress = Kernel::MemoryLayout::BIOS_CALL_CODE_AREA.toAddress();
    const auto size = reinterpret_cast<uint32_t>(&bios_call_16_end) - reinterpret_cast<uint32_t>(&bios_call_16_start);
    targetAddress.copyRange(sourceAddress, size);
}

Kernel::Thread::Context Bios::interrupt(int interruptNumber, const Kernel::Thread::Context &context) {
    auto biosCodeStart = Kernel::MemoryLayout::BIOS_CALL_CODE_AREA.toAddress();

    // Write number of bios interrupt manually into code
    lock.acquire();
    auto interruptNumberAddress = biosCodeStart.add(reinterpret_cast<uint32_t>(&bios_call_16_interrupt) - reinterpret_cast<uint32_t>(&bios_call_16_start));
    interruptNumberAddress.setByte(interruptNumber, 1);

    auto biosContext = protectedModeCall(biosGdt, biosCodeStart.get(), context);

    return lock.releaseAndReturn(biosContext);
}

Kernel::Thread::Context Bios::protectedModeCall(const Kernel::GlobalDescriptorTable &gdt, uint32_t entryPoint, const Kernel::Thread::Context &context) {
    auto &interruptService = Kernel::Service::getService<Kernel::InterruptService>();
    auto &memoryService = Kernel::Service::getService<Kernel::MemoryService>();

    lock.acquire();

    // Get pointer to BIOS context inside lower memory
    auto *biosContext = reinterpret_cast<Kernel::Thread::Context*>(Kernel::MemoryLayout::BIOS_CALL_STACK.endAddress - sizeof(Kernel::Thread::Context) + 1);

    // Copy given context into lower memory
    *biosContext = context;

    // Disable interrupts during the bios call, since our protected mode handler cannot be called
    Cpu::disableInterrupts();
    Cmos::disableNmi();

    // Save interrupt mask
    auto interruptMask = interruptService.getInterruptMask();
    interruptService.setInterruptMask(0x0000);

    // Load BIOS call IDT
    biosIdtDescriptor->load();

    // Switch to bios call GDT
    gdt.load();

    // Call assembly code
    bios_call(biosContext, entryPoint);

    // Switch back to kernel GDT
    memoryService.loadGlobalDescriptorTable();

    // Load kernel IDT
    interruptService.loadIdt();

    // Restore interrupt mask
    interruptService.setInterruptMask(interruptMask);

    // Enable interrupts
    Cmos::enableNmi();
    Cpu::enableInterrupts();

    // Return a copy of the BIOS context
    return lock.releaseAndReturn(*biosContext);
}

}