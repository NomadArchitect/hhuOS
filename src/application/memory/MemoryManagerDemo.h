#ifndef HHUOS_MEMORYMANAGERDEMO_H
#define HHUOS_MEMORYMANAGERDEMO_H


#include "kernel/thread/Thread.h"

class MemoryManagerDemo : public Kernel::Thread {

public:

    MemoryManagerDemo() = default;

    MemoryManagerDemo(const MemoryManagerDemo &copy) = delete;

    MemoryManagerDemo& operator=(const MemoryManagerDemo &copy) = delete;

    ~MemoryManagerDemo() override = default;

    void run() override;

};


#endif
