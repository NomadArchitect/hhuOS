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

#ifndef __Address_include__
#define __Address_include__

#include <cstdint>

namespace Util::Memory {

template<typename T>
class Address {

public:

    Address();

    explicit Address(T address, T limitOffset = static_cast<T>(0xffffffff));

    explicit Address(void *pointer, uint32_t limit = 0xffffffff);

    explicit Address(const void *pointer, uint32_t limit = 0xffffffff);

    ~Address() = default;

    Address(const Address &other) = default;

    Address &operator=(const Address &other) = default;

    bool operator==(const Address &other) const;

    bool operator!=(const Address &other) const;

    bool operator==(T otherAddress) const;

    bool operator!=(T otherAddress) const;

    explicit operator T() const;

    [[nodiscard]] T get() const;

    [[nodiscard]] T getLimit() const;

    [[nodiscard]] Address set(T newAddress) const;

    [[nodiscard]] Address<T> add(T value) const;

    [[nodiscard]] Address<T> subtract(T value) const;

    [[nodiscard]] Address alignUp(T alignment) const;

    [[nodiscard]] T stringLength() const;

    [[nodiscard]] T compareRange(Address<T> otherAddress, T length) const;

    [[nodiscard]] T compareString(Address<T> otherAddress) const;

    [[nodiscard]] uint32_t compareString(const char *otherString) const;

    [[nodiscard]] uint8_t getByte(T offset = 0) const;

    [[nodiscard]] uint16_t getShort(T offset = 0) const;

    [[nodiscard]] uint32_t getInt(T offset = 0) const;

    [[nodiscard]] uint64_t getLong(T offset = 0) const;

    void setByte(uint8_t value, T offset = 0) const;

    void setShort(uint16_t value, T offset = 0) const;

    void setInt(uint32_t value, T offset = 0) const;

    void setLong(uint64_t value, T offset = 0) const;

    void setRange(uint8_t value, T length = 0) const;

    void copyRange(Address<T> sourceAddress, T length) const;

    void copyString(Address<T> sourceAddress) const;

    void copyString(Address<T> sourceAddress, T maxBytes) const;

private:

    T address;
    T limit;
};

template
class Address<uint16_t>;

template
class Address<uint32_t>;

}

#endif