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

#include "ThreadScheduler.h"
#include "ProcessScheduler.h"
#include "kernel/system/System.h"
#include "asm_interface.h"

namespace Kernel {

ThreadScheduler::ThreadScheduler(ProcessScheduler &parent) : parent(parent) {}

ThreadScheduler::~ThreadScheduler() {
    while (!threadQueue.isEmpty()) {
        delete threadQueue.pop();
    }
}

void ThreadScheduler::ready(Thread &thread) {
    if (currentThread == nullptr) {
        currentThread = &thread;
    }

    if (threadQueue.contains(&thread)) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT, "Scheduler: Thread is already running!");
    }

    lock.acquire();
    threadQueue.push(&thread);
    lock.release();
}

void ThreadScheduler::exit() {
    lock.acquire();
    threadQueue.remove(currentThread);
    lock.release();

    if (threadQueue.isEmpty()) {
        parent.exit();
    }

    // TODO: Delete thread (currently not possible, because the instance is still needed for yielding)
    parent.forceYield();
}

void ThreadScheduler::kill(Thread &thread) {
    if (thread.getId() == currentThread->getId()) {
        Util::Exception::throwException(Util::Exception::INVALID_ARGUMENT,"Scheduler: A thread is trying to kill itself... Use 'exitProcess()' instead!");
    }

    lock.acquire();
    threadQueue.remove(&thread);
    lock.release();

    delete &thread;
}

Thread &ThreadScheduler::getCurrentThread() {
    return *currentThread;
}

Thread& ThreadScheduler::getNextThread(bool tryLock) {
    if (!isThreadWaiting()) {
        lock.release();
        parent.yieldFromThreadScheduler(tryLock);
    }

    Thread *thread = threadQueue.pop();
    threadQueue.push(thread);

    return *thread;
}

bool ThreadScheduler::isThreadWaiting() {
    return !threadQueue.isEmpty();
}

void ThreadScheduler::yield(Thread &oldThread, Process &nextProcess, bool tryLock) {
    if (tryLock) {
        if (!lock.tryAcquire()) {
            parent.lock.release();
            return;
        }
    } else {
        lock.acquire();
    }

    Thread &nextThread = getNextThread(false);
    lock.release();

    System::getService<Kernel::MemoryService>().switchAddressSpace(nextProcess.getAddressSpace());
    parent.currentProcess = &nextProcess;

    dispatch(oldThread, nextThread);
}

void ThreadScheduler::dispatch(Thread &current, Thread &next) {
    currentThread = &next;
    switch_context(&current.kernelContext, &next.kernelContext);
}

}