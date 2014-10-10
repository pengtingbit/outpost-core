/*
 * Copyright (c) 2014, German Aerospace Center (DLR)
 * All Rights Reserved.
 *
 * See the file "LICENSE" for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef COBC_UTILS_BITFIELD_IMPL_H
#define COBC_UTILS_BITFIELD_IMPL_H

#include "bitfield.h"

#include "bit_access.h"
#include "serialize.h"

template <int offset>
bool
cobc::Bitfield::read(const uint8_t* byteArray)
{
    const size_t index = offset / 8;
    const int offsetByte = offset & 7;

    bool value = BitAccess::get<uint8_t, 7 - offsetByte>(byteArray[index]);
    return value;
}

template <int start, int end>
uint16_t
cobc::Bitfield::read(const uint8_t* byteArray)
{
    // Get the byte index
    const size_t index = start / 8;

    // Load the bytes in big endian order
    Deserialize stream(&byteArray[index]);
    uint16_t word = stream.read<uint16_t>();

    const int wordOffset = start & 7;
    const int bitCount = end - start;
    const int startWord = 15 - bitCount - wordOffset;
    const int endWord = 15 - wordOffset;
    uint16_t value = BitAccess::get<uint16_t, startWord, endWord>(word);

    return value;
}

template <int offset>
void
cobc::Bitfield::write(uint8_t* byteArray, bool value)
{
    const int index = offset / 8;
    const int bitpos = offset & 7;

    // clear and set bitfield
    byteArray[index] &= static_cast<uint8_t>(~(1 << (7 - bitpos)));
    byteArray[index] |= static_cast<uint8_t>(value << (7 - bitpos));
}

template <int start, int end>
void
cobc::Bitfield::write(uint8_t* byteArray, uint16_t value)
{
    // get the byte index, if odd, round it down, then get position in 16-bit word
    unsigned int index = start / 8;
    uint16_t pos = start & 0x7;

    const uint16_t numberOfBits = end - start + 1;

    // pos of bits to set (in 16-bit word)
    int bitpos    = 16 - (pos + numberOfBits);

    // bitmask for number of bits to set
    uint16_t mask = static_cast<uint16_t>(((1 << numberOfBits) - 1) << bitpos);

    // shift value to desired position
    value         = static_cast<uint16_t>(value << bitpos);

    // make sure to load the bytes in big endian order
    uint16_t word = (static_cast<uint16_t>(byteArray[index]) << 8) |
                     static_cast<uint16_t>(byteArray[index + 1]);

    // clear bit field
    word = static_cast<uint16_t>(word & ~mask);

    // set the corresponding bits
    word = static_cast<uint16_t>(word | (mask & value));

    // make sure to store the bytes in big-endian order
    // most significant first
    byteArray[index]     = static_cast<uint8_t>(word >> 8);
    byteArray[index + 1] = static_cast<uint8_t>(word);
}

#endif // COBC_UTILS_BITFIELD_IMPL_H