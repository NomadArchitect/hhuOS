#include "device/cpu/Cpu.h"
#include "Atomic.h"

namespace Util::Async {

/*
 * Dummy functions used to return true/false from assembly.
 * These functions should not be called. They are just wrappers for labels, directly jumped at from assembly code.
 */

bool dummyUnset() {
    asm volatile(
    "CF_UNSET:"
    );
    return false;
}

bool dummySet() {
    asm volatile(
    "CF_SET:"
    );
    return true;
}

template<typename T>
Atomic<T>::Atomic(T &value) : value(value) {}

template<typename T>
void Atomic<T>::exchange(volatile void *ptr, T newValue) {
    asm volatile (
    "lock xchg %0, %1"
    :
    : "q"(newValue), "m"(*(volatile T *) ptr)
    : "memory"
    );
}

template<typename T>
T Atomic<T>::fetchAndAdd(volatile void *ptr, T addend) {
    asm volatile (
    "lock xadd %0, %1"
    : "+r" (addend), "+m" (*(volatile T *) ptr)
    :
    : "memory"
    );

    return addend;
}

template<typename T>
T Atomic<T>::compareAndExchange(volatile void *ptr, T oldValue, T newValue) {
    T ret;

    asm volatile (
    "lock cmpxchg %2, %1"
    : "=a"(ret), "+m"(*(volatile T *) ptr)
    : "r"(newValue), "0"(oldValue)
    : "memory"
    );

    return ret;
}

template<typename T>
Atomic<T>::operator T() const {
    return get();
}

template<typename T>
T Atomic<T>::get() const {
    return value;
}

template<typename T>
T Atomic<T>::getAndSet(T newValue) {
    return compareAndExchange(&value, value, newValue);
}

template<typename T>
void Atomic<T>::set(T newValue) {
    exchange(&value, newValue);
}

template<typename T>
T Atomic<T>::fetchAndAdd(T addend) {
    return fetchAndAdd(&value, addend);
}

template<typename T>
T Atomic<T>::fetchAndSub(T subtrahend) {
    return fetchAndAdd(&value, subtrahend * (-1));
}

template<typename T>
T Atomic<T>::fetchAndInc() {
    return fetchAndAdd(&value, 1);
}

template<typename T>
T Atomic<T>::fetchAndDec() {
    return fetchAndAdd(&value, -1);
}

template<typename T>
bool Atomic<T>::bitTest(T index) {
    asm volatile (
    "bt %1, %0;"
    "jc CF_SET;"
    "jmp CF_UNSET;"
    : "+m"(value)
    : "r"(index)
    );

    // Should never be executed
    return true;
}

template<typename T>
void Atomic<T>::bitSet(T index) {
    asm volatile (
    "lock bts %1, %0"
    : "+m"(value)
    : "r"(index)
    );
}

template<typename T>
void Atomic<T>::bitReset(T index) {
    asm volatile (
    "lock btr %1, %0"
    : "+m"(value)
    : "r"(index)
    );
}

template<typename T>
bool Atomic<T>::bitTestAndSet(T index) {
    asm volatile (
    "lock bts %1, %0;"
    "jc CF_SET;"
    "jmp CF_UNSET;"
    : "+m"(value)
    : "r"(index)
    );

    // Should never be executed
    return true;
}

template<typename T>
bool Atomic<T>::bitTestAndReset(T index) {
    asm volatile (
    "lock btr %1, %0;"
    "jc CF_SET;"
    "jmp CF_UNSET;"
    : "+m"(value)
    : "r"(index)
    );

    // Should never be executed
    return true;
}

}