/*
 * Copyright (c) 2013, German Aerospace Center (DLR)
 *
 * This file is part of libCOBC 0.4.
 *
 * It is distributed under the terms of the GNU General Public License with a
 * linking exception. See the file "LICENSE" for the full license governing
 * this code.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */
// ----------------------------------------------------------------------------

#ifndef COBC_UTILS_SERIALIZE_LITTLE_ENDIAN_H
#define COBC_UTILS_SERIALIZE_LITTLE_ENDIAN_H

#include <stddef.h>
#include <string.h>
#include <stdint.h>

#include "bounded_array.h"

namespace cobc
{

/**
 * Serialize data in little endian byte order
 *
 * \author Fabian Greif
 */
// LCOV_EXCL_START
// Functions are tested in \c test/unit/test_byteorder.cpp
// But as they are inline functions lcov can't generate useful
// coverage reports for them
class SerializeLittleEndian
{
public:
    explicit inline
    SerializeLittleEndian(cobc::BoundedArray<uint8_t> array) :
        mBuffer(&array[0]), mBegin(&array[0])
    {
    }

    explicit inline
    SerializeLittleEndian(uint8_t* outputBuffer) :
        mBuffer(outputBuffer), mBegin(outputBuffer)
    {
    }

    inline
    ~SerializeLittleEndian()
    {
    }

    inline
    SerializeLittleEndian(const SerializeLittleEndian& other) :
        mBuffer(other.mBuffer), mBegin(other.mBegin)
    {
    }

    /**
     * Reset the read pointer to the beginning of the
     * originating buffer.
     */
    inline void
    reset()
    {
        mBuffer = mBegin;
    }

    inline void
    store8(const uint8_t data)
    {
        mBuffer[0] = data;
        mBuffer += 1;
    }

    inline void
    store16(const uint16_t data)
    {
        mBuffer[1] = static_cast<uint8_t>(data >> 8);
        mBuffer[0] = static_cast<uint8_t>(data >> 0);
        mBuffer += 2;
    }

    inline void
    store24(const uint32_t data)
    {
        mBuffer[2] = static_cast<uint8_t>(data >> 16);
        mBuffer[1] = static_cast<uint8_t>(data >> 8);
        mBuffer[0] = static_cast<uint8_t>(data >> 0);
        mBuffer += 3;
    }

    inline void
    store32(const uint32_t data)
    {
        mBuffer[3] = static_cast<uint8_t>(data >> 24);
        mBuffer[2] = static_cast<uint8_t>(data >> 16);
        mBuffer[1] = static_cast<uint8_t>(data >> 8);
        mBuffer[0] = static_cast<uint8_t>(data >> 0);
        mBuffer += 4;
    }

    inline void
    store64(const uint64_t data)
    {
        mBuffer[7] = static_cast<uint8_t>(data >> 56);
        mBuffer[6] = static_cast<uint8_t>(data >> 48);
        mBuffer[5] = static_cast<uint8_t>(data >> 40);
        mBuffer[4] = static_cast<uint8_t>(data >> 32);
        mBuffer[3] = static_cast<uint8_t>(data >> 24);
        mBuffer[2] = static_cast<uint8_t>(data >> 16);
        mBuffer[1] = static_cast<uint8_t>(data >> 8);
        mBuffer[0] = static_cast<uint8_t>(data >> 0);
        mBuffer += 8;
    }

    /**
     * Store two 12 bit values in a three byte array.
     *
     * The 12 bit value need to be stored in the lower 12 bit of the
     * two 16 bit parameters.
     */
    inline void
    storePacked12(const uint16_t first, const uint16_t second)
    {
        mBuffer[0] = static_cast<uint8_t>(first >> 0);
        mBuffer[1] = static_cast<uint8_t>(((first >> 4) & 0xF0) | (second & 0x0F));
        mBuffer[2] = static_cast<uint8_t>(second >> 4);
        mBuffer += 3;
    }

    inline void
    storeFloat(const float data)
    {
        const uint32_t* ptr = reinterpret_cast<const uint32_t *>(&data);
        store32(*ptr);
    }

    inline void
    storeDouble(const double data)
    {
        const uint64_t* ptr = reinterpret_cast<const uint64_t *>(&data);
        store64(*ptr);
    }

    inline void
    storeBuffer(const uint8_t* buffer, const size_t length)
    {
        memcpy(mBuffer, buffer, length);
        mBuffer += length;
    }

    inline void
    storeBuffer(cobc::BoundedArray<const uint8_t> array)
    {
        size_t length = array.getNumberOfElements();
        memcpy(mBuffer, &array[0], length);
        mBuffer += length;
    }

    // explicit template instantiations are provided in serialize_impl.h
    template<typename T>
    inline void
    store(T data);

    template<typename T>
    inline void
    storeObject(const T& data);

    /**
     * Skip forward the given number of bytes.
     *
     * \param bytes
     *      Number of byte to skip forward.
     */
    inline void
    skip(const size_t bytes)
    {
        mBuffer += bytes;
    }

    template<typename T>
    inline void
    skip()
    {
        mBuffer += sizeof(T);
    }

    inline uint8_t*
    getPointer()
    {
        return mBegin;
    }

    // get position of the buffer
    inline ptrdiff_t
    getPosition() const
    {
        return (mBuffer - mBegin);
    }

    inline uint8_t*
    getPointerToCurrentPosition()
    {
        return mBuffer;
    }

    template<typename T>
    inline SerializeLittleEndian&
    operator<<(const T& data)
    {
        store<T>(data);
        return *this;
    }

private:
    // disable assignment operator
    SerializeLittleEndian&
    operator=(const SerializeLittleEndian& other);

    uint8_t* mBuffer;
    uint8_t* mBegin;
};
// LCOV_EXCL_STOP
// LCOV_EXCL_END

// ----------------------------------------------------------------------------
/**
 * Deserialize
 *
 * The read8(), read16() and read32() functions read the number of bits
 * from the current location and move the data pointer forward
 * correspondingly. The peek8(), peek16() and peek32() read a value
 * n bytes in front of the current location and *don't* move the
 * data pointer.
 *
 * \author Fabian Greif
 */
class DeserializeLittleEndian
{
public:
    explicit inline
    DeserializeLittleEndian(const uint8_t* inputBuffer) :
        mBuffer(inputBuffer),
        mBegin(inputBuffer)
    {
    }

    explicit inline
    DeserializeLittleEndian(cobc::BoundedArray<const uint8_t> array) :
        mBuffer(&array[0]),
        mBegin(&array[0])
    {
    }

    inline
    ~DeserializeLittleEndian()
    {
    }

    inline
    DeserializeLittleEndian(const DeserializeLittleEndian& other) :
        mBuffer(other.mBuffer),
        mBegin(other.mBegin)
    {
    }

    /**
     * Reset the read pointer to the beginning of the
     * originating buffer.
     */
    inline void
    reset()
    {
        mBuffer = mBegin;
    }

    inline uint8_t
    read8()
    {
        uint8_t value;
        value = mBuffer[0];
        mBuffer += 1;

        return value;
    }

    inline uint8_t
    peek8(const size_t n) const
    {
        return mBuffer[n];
    }

    inline uint16_t
    read16()
    {
        uint16_t value = 0;
        value |= static_cast<uint16_t>(mBuffer[1]) << 8;
        value |= static_cast<uint16_t>(mBuffer[0]) << 0;
        mBuffer += 2;

        return value;
    }

    inline uint16_t
    peek16(const size_t n) const
    {
        uint16_t value = 0;
        value |= static_cast<uint16_t>(mBuffer[n + 1]) << 8;
        value |= static_cast<uint16_t>(mBuffer[n + 0]) << 0;

        return value;
    }

    inline uint32_t
    read24()
    {
        uint32_t value = 0;
        value |= static_cast<uint32_t>(mBuffer[2]) << 16;
        value |= static_cast<uint32_t>(mBuffer[1]) << 8;
        value |= static_cast<uint32_t>(mBuffer[0]) << 0;
        mBuffer += 3;

        return value;
    }

    inline uint32_t
    peek24(const size_t n) const
    {
        uint32_t value = 0;
        value |= static_cast<uint32_t>(mBuffer[n + 2]) << 16;
        value |= static_cast<uint32_t>(mBuffer[n + 1]) << 8;
        value |= static_cast<uint32_t>(mBuffer[n + 0]) << 0;

        return value;
    }

    inline uint32_t
    read32()
    {
        uint32_t value = 0;
        value |= static_cast<uint32_t>(mBuffer[3]) << 24;
        value |= static_cast<uint32_t>(mBuffer[2]) << 16;
        value |= static_cast<uint32_t>(mBuffer[1]) << 8;
        value |= static_cast<uint32_t>(mBuffer[0]) << 0;
        mBuffer += 4;

        return value;
    }

    inline uint32_t
    peek32(const size_t n) const
    {
        uint32_t value = 0;
        value |= static_cast<uint32_t>(mBuffer[n + 3]) << 24;
        value |= static_cast<uint32_t>(mBuffer[n + 2]) << 16;
        value |= static_cast<uint32_t>(mBuffer[n + 1]) << 8;
        value |= static_cast<uint32_t>(mBuffer[n + 0]) << 0;

        return value;
    }

    inline uint64_t
    read64()
    {
        uint64_t value = 0;
        value |= static_cast<uint64_t>(mBuffer[7]) << 56;
        value |= static_cast<uint64_t>(mBuffer[6]) << 48;
        value |= static_cast<uint64_t>(mBuffer[5]) << 40;
        value |= static_cast<uint64_t>(mBuffer[4]) << 32;
        value |= static_cast<uint64_t>(mBuffer[3]) << 24;
        value |= static_cast<uint64_t>(mBuffer[2]) << 16;
        value |= static_cast<uint64_t>(mBuffer[1]) << 8;
        value |= static_cast<uint64_t>(mBuffer[0]) << 0;
        mBuffer += 8;

        return value;
    }

    inline uint64_t
    peek64(const size_t n) const
    {
        uint64_t value = 0;
        value |= static_cast<uint64_t>(mBuffer[n + 7]) << 56;
        value |= static_cast<uint64_t>(mBuffer[n + 6]) << 48;
        value |= static_cast<uint64_t>(mBuffer[n + 5]) << 40;
        value |= static_cast<uint64_t>(mBuffer[n + 4]) << 32;
        value |= static_cast<uint64_t>(mBuffer[n + 3]) << 24;
        value |= static_cast<uint64_t>(mBuffer[n + 2]) << 16;
        value |= static_cast<uint64_t>(mBuffer[n + 1]) << 8;
        value |= static_cast<uint64_t>(mBuffer[n + 0]) << 0;

        return value;
    }

    /**
     * Read two 12 bit values from a three byte array.
     *
     * The 12 bit value are stored in the lower 12 bit of the two 16 bit
     * parameters.
     */
    inline void
    readPacked12(uint16_t& first, uint16_t& second)
    {
        first  = static_cast<uint16_t>(mBuffer[0]);
        first |= static_cast<uint16_t>(mBuffer[1] & 0xF0) << 4;

        second  = static_cast<uint16_t>(mBuffer[1] & 0x0F);
        second |= static_cast<uint16_t>(mBuffer[2]) << 4;
        mBuffer += 3;
    }

    inline void
    peekPacked12(const size_t n, uint16_t& first, uint16_t& second)
    {
        first  = static_cast<uint16_t>(mBuffer[n + 0]);
        first |= static_cast<uint16_t>(mBuffer[n + 1] & 0xF0) << 4;

        second  = static_cast<uint16_t>(mBuffer[n + 1] & 0x0F);
        second |= static_cast<uint16_t>(mBuffer[n + 2]) << 4;
    }

    inline float
    readFloat(void)
    {
        float f;
        const uint32_t value = read32();

        memcpy(&f, &value, sizeof(f));
        return f;
    }

    inline float
    peekFloat(const size_t n) const
    {
        float f;
        const uint32_t value = peek32(n);

        memcpy(&f, &value, sizeof(f));
        return f;
    }

    inline double
    readDouble(void)
    {
        double d;
        const uint64_t value = read64();

        memcpy(&d, &value, sizeof(d));
        return d;
    }

    inline double
    peekDouble(const size_t n) const
    {
        double d;
        const uint64_t value = peek64(n);

        memcpy(&d, &value, sizeof(d));
        return d;
    }

    inline void
    readBuffer(uint8_t* buffer, const size_t length)
    {
        memcpy(buffer, mBuffer, length);
        mBuffer += length;
    }

    inline void
    peekBuffer(uint8_t* buffer, const size_t length)
    {
        memcpy(buffer, mBuffer, length);
    }

    template<typename T>
    inline T
    peek(const size_t n) const;

    template<typename T>
    inline T
    read();

    /**
     * Skip forward the given number of bytes.
     *
     * \param bytes
     *      Number of byte to skip forward.
     */
    inline void
    skip(const size_t bytes)
    {
        mBuffer += bytes;
    }

    template<typename T>
    inline void
    skip()
    {
        mBuffer += sizeof(T);
    }

    template<typename T>
    inline DeserializeLittleEndian&
    operator>>(T& data)
    {
        data = read<T>();
        return *this;
    }

    inline const uint8_t*
    getPointer() const
    {
        return mBegin;
    }

    /**
     * Get Pointer to the current location in the buffer.
     */
    inline const uint8_t*
    getPointerToCurrentPosition() const
    {
        return mBuffer;
    }

    inline ptrdiff_t
    getPosition() const
    {
        return (mBuffer - mBegin);
    }

    template<typename T>
    inline T
    getPosition() const
    {
        return static_cast<T>(mBuffer - mBegin);
    }

private:
    // disable assignment operator
    DeserializeLittleEndian&
    operator=(const DeserializeLittleEndian& other);

    const uint8_t* mBuffer;
    const uint8_t* const mBegin;
};
}

#include "serialize_little_endian_impl.h"

#endif