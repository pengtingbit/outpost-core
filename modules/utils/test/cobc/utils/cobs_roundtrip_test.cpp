/*
 * Copyright (c) 2014, German Aerospace Center (DLR)
 *
 * This file is part of libCOBC 0.6.
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

#include <cobc/utils/cobs.h>

#include <string.h>     // for memset
#include <unittest/harness.h>

#include <rapidcheck/gtest.h>

using Cobs = cobc::utils::Cobs<254>;
using ::testing::ElementsAreArray;

class CobsRoundtriptTest : public ::testing::Test
{
public:
    virtual void
    SetUp()
    {
        memset(encoded, 0xAB, sizeof(encoded));
        memset(actual,  0xAB, sizeof(actual));
    }

    size_t
    roundtrip(cobc::BoundedArray<uint8_t> input)
    {
        size_t encodedLength = Cobs::encode(input, cobc::toArray(encoded));
        size_t decodedLength = Cobs::decode(cobc::BoundedArray<uint8_t>(encoded, encodedLength), actual);

        return decodedLength;
    }

    uint8_t encoded[1024];
    uint8_t actual[1024];
};

// ----------------------------------------------------------------------------
TEST_F(CobsRoundtriptTest, singleZeroByte)
{
    uint8_t input[1] = { 0 };

    uint8_t expected[3] = { 0, 0xAB, 0xAB };

    size_t decodedLength = roundtrip(cobc::toArray(input));

    EXPECT_EQ(sizeof(input), decodedLength);
    EXPECT_THAT(expected, ElementsAreArray(actual, 3));
}

TEST_F(CobsRoundtriptTest, zeroPrefixAndSuffix)
{
    uint8_t input[3] = { 0, 1, 0 };

    uint8_t expected[5] = { 0, 1, 0, 0xAB, 0xAB };

    size_t decodedLength = roundtrip(cobc::toArray(input));

    EXPECT_EQ(sizeof(input), decodedLength);
    EXPECT_THAT(expected, ElementsAreArray(actual, 5));
}

TEST_F(CobsRoundtriptTest, blockOfDataWithoutZero)
{
    uint8_t input[512];
    for (size_t i = 0; i < 512; ++i)
    {
        input[i] = (i % 255) + 1;
    }

    size_t decodedLength = roundtrip(cobc::toArray(input));

    EXPECT_EQ(sizeof(input), decodedLength);
    EXPECT_THAT(input, ElementsAreArray(actual, sizeof(input)));
}

// ----------------------------------------------------------------------------
/*
 * Decoded the COBS data in place by using the same array for input and
 * output of the encoder.
 */
TEST_F(CobsRoundtriptTest, inPlaceDecodingOfBlockOfDataWithoutZero)
{
    uint8_t input[512];
    for (size_t i = 0; i < 512; ++i)
    {
        input[i] = (i % 255) + 1;
    }

    size_t encodedLength = Cobs::encode(cobc::toArray(input), cobc::toArray(encoded));
    size_t decodedLength = Cobs::decode(cobc::BoundedArray<uint8_t>(encoded, encodedLength), encoded);

    EXPECT_EQ(sizeof(input), decodedLength);
    EXPECT_THAT(input, ElementsAreArray(encoded, sizeof(input)));
}

TEST_F(CobsRoundtriptTest, inPlaceDecodingOfZeroPrefixAndSuffix)
{
    uint8_t input[3] = { 0, 1, 0 };

    size_t encodedLength = Cobs::encode(cobc::toArray(input), cobc::toArray(encoded));
    size_t decodedLength = Cobs::decode(cobc::BoundedArray<uint8_t>(encoded, encodedLength), encoded);

    EXPECT_EQ(sizeof(input), decodedLength);
    EXPECT_THAT(input, ElementsAreArray(encoded, sizeof(input)));
}

RC_GTEST_FIXTURE_PROP(CobsRoundtriptTest, shouldPerformRoundTripWithRandomData, ())
{
	const auto input = *rc::gen::resize(200, rc::gen::arbitrary<std::vector<uint8_t>>());

	cobc::BoundedArray<const uint8_t> inputArray(&input[0], input.size());
	size_t encodedLength = cobc::utils::Cobs<32>::encode(inputArray, cobc::toArray(encoded));
	size_t decodedLength = cobc::utils::Cobs<32>::decode(cobc::BoundedArray<uint8_t>(encoded, encodedLength), encoded);

	RC_ASSERT(input.size() == decodedLength);
	RC_ASSERT(input == std::vector<uint8_t>(encoded, &encoded[decodedLength]));
}
