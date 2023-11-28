// Import of standard or provided libraries
#include <gtest/gtest.h>
#include <cmath>
#include <string>

// Import of local libraries
#include <golomb.h>

#define TEST_FILE_NAME "test_golomb.bin"

using namespace std;

/*
##############################################################################
###################           SIGN MAGNITUDE               ###################
##############################################################################
*/

TEST(Golomb, smallValues_SignMagnitude_mPowerOf2) {

    const int MAX_VALUE = 200;

    for (int m = 2; m <= MAX_VALUE; m += 2) {

        BitStream writer('w', TEST_FILE_NAME);
        Golomb golomb(m, writer);
        std::list<int> values;
        for (int i = 0; i <= MAX_VALUE; i++) {
            golomb.encode(i);
            values.push_back(i);
        }
        golomb.~Golomb();

        BitStream reader('r', TEST_FILE_NAME);
        Golomb golomb2(m, reader);
        for (int i = 0; i <= MAX_VALUE; i++) {
            int decoded = golomb2.decode();
            int original = values.front();

            values.pop_front();
            EXPECT_EQ(decoded, original);
        }
        EXPECT_EQ(values.size(), 0);

        golomb2.~Golomb();

        remove(TEST_FILE_NAME);
    }
};

TEST(Golomb, negativeSmallValues_SignMagnitude_mPowerOf2) {

    const int MIN_VALUE = -200;

    for (int m = 2; m <= 200; m += 2) {

        BitStream writer('w', TEST_FILE_NAME);
        Golomb golomb(4, writer, SIGN_MAGNITUDE);
        std::list<int> values;
        for (int i = MIN_VALUE; i < 0; i++) {
            golomb.encode(i);
            values.push_back(i);
        }
        golomb.~Golomb();

        BitStream reader('r', TEST_FILE_NAME);
        Golomb golomb2(4, reader, SIGN_MAGNITUDE);
        for (int i = MIN_VALUE; i < 0; i++) {
            int decoded = golomb2.decode();
            int original = values.front();

            values.pop_front();
            EXPECT_EQ(decoded, original);
        }
        EXPECT_EQ(values.size(), 0);

        golomb2.~Golomb();

        remove(TEST_FILE_NAME);
    }
};

TEST(Golomb, intensive_SignMagnitude_mPowerOf2) {

    const int MAX_VALUE = 32764, MIN_VALUE = -32764;

    BitStream writer('w', TEST_FILE_NAME);
    Golomb golomb(128, writer, SIGN_MAGNITUDE);

    for (int i = MIN_VALUE; i <= MAX_VALUE; i++)
        golomb.encode(i);

    golomb.~Golomb();

    BitStream reader('r', TEST_FILE_NAME);
    Golomb golomb2(128, reader, SIGN_MAGNITUDE);

    for (int i = MIN_VALUE; i <= MAX_VALUE; i++) {
        int decoded = golomb2.decode();
        EXPECT_EQ(decoded, i);
    }

    golomb2.~Golomb();

    remove(TEST_FILE_NAME);
};

TEST(Golomb, smallValues_SignMagnitude_mNotPowerOf2) {

    const int MAX_VALUE = 20;

    BitStream writer('w', TEST_FILE_NAME);
    Golomb golomb(5, writer);
    std::list<int> values;
    for (int i = 0; i <= MAX_VALUE; i++) {
        golomb.encode(i);
        values.push_back(i);
    }
    golomb.~Golomb();

    BitStream reader('r', TEST_FILE_NAME);
    Golomb golomb2(5, reader);
    for (int i = 0; i <= MAX_VALUE; i++) {
        int decoded = golomb2.decode();
        int original = values.front();

        values.pop_front();
        EXPECT_EQ(decoded, original);
    }
    EXPECT_EQ(values.size(), 0);

    golomb2.~Golomb();

    remove(TEST_FILE_NAME);
};

TEST(Golomb, negativeSmallValues_SignMagnitude_mNotPowerOf2) {

    const int MIN_VALUE = -200;

    BitStream writer('w', TEST_FILE_NAME);
    Golomb golomb(5, writer, SIGN_MAGNITUDE);
    std::list<int> values;
    for (int i = MIN_VALUE; i < 0; i++) {
        golomb.encode(i);
        values.push_back(i);
    }
    golomb.~Golomb();

    BitStream reader('r', TEST_FILE_NAME);
    Golomb golomb2(5, reader, SIGN_MAGNITUDE);
    for (int i = MIN_VALUE; i < 0; i++) {
        int decoded = golomb2.decode();
        int original = values.front();

        values.pop_front();
        EXPECT_EQ(decoded, original);
    }
    EXPECT_EQ(values.size(), 0);

    golomb2.~Golomb();

    remove(TEST_FILE_NAME);
};

TEST(Golomb, intensive_SignMagnitude_mNotPowerOf2) {

    const int MAX_VALUE = 32764, MIN_VALUE = -32764;

    BitStream writer('w', TEST_FILE_NAME);
    Golomb golomb(100, writer, SIGN_MAGNITUDE);

    for (int i = MIN_VALUE; i <= MAX_VALUE; i++)
        golomb.encode(i);

    golomb.~Golomb();

    BitStream reader('r', TEST_FILE_NAME);
    Golomb golomb2(100, reader, SIGN_MAGNITUDE);

    for (int i = MIN_VALUE; i <= MAX_VALUE; i++) {
        int decoded = golomb2.decode();
        EXPECT_EQ(decoded, i);
    }

    golomb2.~Golomb();

    remove(TEST_FILE_NAME);
};

/*
##############################################################################
###################          VALUE INTERLEAVING            ###################
##############################################################################
*/

TEST(Golomb, smallValues_ValueInterleaving_mPowerOf2) {

    const int MAX_VALUE = 20;

    BitStream writer('w', TEST_FILE_NAME);
    Golomb golomb(4, writer, VALUE_INTERLEAVING);
    std::list<int> values;
    for (int i = 0; i <= MAX_VALUE; i++) {
        golomb.encode(i);
        values.push_back(i);
    }
    golomb.~Golomb();

    BitStream reader('r', TEST_FILE_NAME);
    Golomb golomb2(4, reader, VALUE_INTERLEAVING);
    for (int i = 0; i <= MAX_VALUE; i++) {
        int decoded = golomb2.decode();
        int original = values.front();

        values.pop_front();
        EXPECT_EQ(decoded, original);
    }
    EXPECT_EQ(values.size(), 0);

    golomb2.~Golomb();

    remove(TEST_FILE_NAME);
};

TEST(Golomb, negativeSmallValues_ValueInterleaving_mPowerOf2) {

    const int MIN_VALUE = -200;

    BitStream writer('w', TEST_FILE_NAME);
    Golomb golomb(4, writer, VALUE_INTERLEAVING);
    std::list<int> values;
    for (int i = MIN_VALUE; i < 0; i++) {
        golomb.encode(i);
        values.push_back(i);
    }
    golomb.~Golomb();

    BitStream reader('r', TEST_FILE_NAME);
    Golomb golomb2(4, reader, VALUE_INTERLEAVING);
    for (int i = MIN_VALUE; i < 0; i++) {
        int decoded = golomb2.decode();
        int original = values.front();

        values.pop_front();
        EXPECT_EQ(decoded, original);
    }
    EXPECT_EQ(values.size(), 0);

    golomb2.~Golomb();

    remove(TEST_FILE_NAME);
};

TEST(Golomb, intensive_ValueInterleaving_mPowerOf2) {

    const int MAX_VALUE = 32764, MIN_VALUE = -32764;

    BitStream writer('w', TEST_FILE_NAME);
    Golomb golomb(128, writer, VALUE_INTERLEAVING);

    for (int i = MIN_VALUE; i <= MAX_VALUE; i++)
        golomb.encode(i);

    golomb.~Golomb();

    BitStream reader('r', TEST_FILE_NAME);
    Golomb golomb2(128, reader, VALUE_INTERLEAVING);

    for (int i = MIN_VALUE; i <= MAX_VALUE; i++) {
        int decoded = golomb2.decode();
        EXPECT_EQ(decoded, i);
    }

    golomb2.~Golomb();

    remove(TEST_FILE_NAME);
};

TEST(Golomb, smallValues_ValueInterleaving_mNotPowerOf2) {

    const int MAX_VALUE = 20;

    BitStream writer('w', TEST_FILE_NAME);
    Golomb golomb(5, writer, VALUE_INTERLEAVING);
    std::list<int> values;
    for (int i = 0; i <= MAX_VALUE; i++) {
        golomb.encode(i);
        values.push_back(i);
    }
    golomb.~Golomb();

    BitStream reader('r', TEST_FILE_NAME);
    Golomb golomb2(5, reader, VALUE_INTERLEAVING);
    for (int i = 0; i <= MAX_VALUE; i++) {
        int decoded = golomb2.decode();
        int original = values.front();

        values.pop_front();
        EXPECT_EQ(decoded, original);
    }
    EXPECT_EQ(values.size(), 0);

    golomb2.~Golomb();

    remove(TEST_FILE_NAME);
};

TEST(Golomb, negativeSmallValues_ValueInterleaving_mNotPowerOf2) {

    const int MIN_VALUE = -200;

    BitStream writer('w', TEST_FILE_NAME);
    Golomb golomb(5, writer, VALUE_INTERLEAVING);
    std::list<int> values;
    for (int i = MIN_VALUE; i < 0; i++) {
        golomb.encode(i);
        values.push_back(i);
    }
    golomb.~Golomb();

    BitStream reader('r', TEST_FILE_NAME);
    Golomb golomb2(5, reader, VALUE_INTERLEAVING);
    for (int i = MIN_VALUE; i < 0; i++) {
        int decoded = golomb2.decode();
        int original = values.front();

        values.pop_front();
        EXPECT_EQ(decoded, original);
    }
    EXPECT_EQ(values.size(), 0);

    golomb2.~Golomb();

    remove(TEST_FILE_NAME);
};

TEST(Golomb, intensive_ValueInterleaving_mNotPowerOf2) {

    const int MAX_VALUE = 32764, MIN_VALUE = -32764;

    BitStream writer('w', TEST_FILE_NAME);
    Golomb golomb(100, writer, VALUE_INTERLEAVING);

    for (int i = MIN_VALUE; i <= MAX_VALUE; i++)
        golomb.encode(i);

    golomb.~Golomb();

    BitStream reader('r', TEST_FILE_NAME);
    Golomb golomb2(100, reader, VALUE_INTERLEAVING);

    for (int i = MIN_VALUE; i <= MAX_VALUE; i++) {
        int decoded = golomb2.decode();
        EXPECT_EQ(decoded, i);
    }

    golomb2.~Golomb();

    remove(TEST_FILE_NAME);
};