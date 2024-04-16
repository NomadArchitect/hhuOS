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

#ifndef HHUOS_THREAD_H
#define HHUOS_THREAD_H

#include <cstdint>

#include "lib/util/base/String.h"

namespace Util {
namespace Async {
class Runnable;
template <typename T> class IdGenerator;
}  // namespace Async
}  // namespace Util

namespace Kernel {

class Process;

class Thread {

    friend class Scheduler;

public:

    struct Context {
        uint32_t ds;
        uint32_t es;
        uint32_t fs;
        uint32_t gs;
        uint32_t flags;
        uint32_t edi;
        uint32_t esi;
        uint32_t ebp;
        uint32_t esp;
        uint32_t ebx;
        uint32_t edx;
        uint32_t ecx;
        uint32_t eax;
    } __attribute__((packed));

    /**
     * Copy Constructor.
     */
    Thread(const Thread &other) = delete;

    /**
     * Assignment operator.
     */
    Thread &operator=(const Thread &other) = delete;

    /**
     * Destructor.
     */
    virtual ~Thread();

    static Thread& createKernelThread(const Util::String &name, Process &parent, Util::Async::Runnable *runnable);

    static Thread &createUserThread(const Util::String &name, Process &parent, uint32_t eip, Util::Async::Runnable *runnable);

    static Thread& createMainUserThread(const Util::String &name, Process &parent, uint32_t eip, uint32_t argc, char **argv, void *envp, uint32_t heapStartAddress);

    [[nodiscard]] uint32_t getId() const;

    [[nodiscard]] Util::String getName() const;

    [[nodiscard]] Process& getParent() const;

    [[nodiscard]] uint8_t* getFpuContext() const;

    [[nodiscard]] bool isKernelThread() const;

    void join();

    virtual void run();

    static void startFirstThread(const Thread &thread);

    static void switchThread(Thread &current, const Thread &next);

private:

    Thread(const Util::String &name, Process &parent, Util::Async::Runnable *runnable, uint32_t userInstructionPointer, uint32_t *kernelStack, uint32_t *userStack);

    void prepareKernelStack();

    static void kickoffKernelThread();

    void switchToUserMode();

    static uint32_t* createKernelStack(uint32_t size);

    static uint32_t* createUserStack(uint32_t size);

    static uint32_t* createMainUserStack();

    uint32_t id;
    Util::String name;
    Process &parent;
    Util::Async::Runnable *runnable;
    uint32_t userInstructionPointer;

    uint32_t *kernelStack;
    uint32_t *userStack;
    uint32_t *oldStackPointer;

    uint8_t *fpuContext;

    static uint8_t defaultFpuContext[512];

    static Util::Async::IdGenerator<uint32_t> idGenerator;
    static const constexpr uint32_t STACK_SIZE = 4096;
};

}

#endif
