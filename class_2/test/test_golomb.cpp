// Import of standard or provided libraries
#include <gtest/gtest.h>
#include <cmath>
#include <string>

// Import of local libraries
#include <golomb.h>

#define TEST_FILE_NAME "test_golomb.bin"

using namespace std;

TEST(Golomb, smallValues_mPowerOf2) {

    BitStream writer('w', TEST_FILE_NAME);
    Golomb golomb(4, writer);
    std::list<int> values;
    for (int i = 0; i <= 20; i += 2) {
        golomb.encode(i);
        values.push_back(i);
    }
    golomb.~Golomb();

    BitStream reader('r', TEST_FILE_NAME);
    Golomb golomb2(4, reader);
    for (int i = 0; i <= 20; i += 2) {
        int decoded = golomb2.decode();
        int original = values.front();

        values.pop_front();
        EXPECT_EQ(decoded, original);
    }
    EXPECT_EQ(values.size(), 0);

    golomb2.~Golomb();

    remove(TEST_FILE_NAME);
};

TEST(Golomb, smallValues_mNotPowerOf2) {

    BitStream writer('w', TEST_FILE_NAME);
    Golomb golomb(5, writer, SIGN_MAGNITUDE);
    std::list<int> values;
    for (int i = 1; i <= 30; i += 2) {
        golomb.encode(i);
        values.push_back(i);
    }
    golomb.~Golomb();

    BitStream reader('r', TEST_FILE_NAME);
    Golomb golomb2(5, reader);
    for (int i = 1; i <= 30; i += 2) {
        int decoded = golomb2.decode();
        int original = values.front();

        values.pop_front();
        EXPECT_EQ(decoded, original);
    }
    EXPECT_EQ(values.size(), 0);

    golomb2.~Golomb();

    remove(TEST_FILE_NAME);
};

TEST(Golomb, negativeSignMagnitude) {

    BitStream writer('w', TEST_FILE_NAME);
    Golomb golomb(11, writer);
    std::list<int> values;
    for (int i = -1; i >= -15; i--) {
        golomb.encode(i);
        values.push_back(i);
    }
    golomb.~Golomb();

    BitStream reader('r', TEST_FILE_NAME);
    Golomb golomb2(11, reader);
    for (int i = -1; i >= -15; i--) {
        int decoded = golomb2.decode();
        int original = values.front();

        values.pop_front();
        EXPECT_EQ(decoded, original);
    }
    EXPECT_EQ(values.size(), 0);

    golomb2.~Golomb();

    remove(TEST_FILE_NAME);
};

TEST(Golomb, negativeValueInterleaving) {

    BitStream writer('w', TEST_FILE_NAME);
    Golomb golomb(5, writer, VALUE_INTERLEAVING);
    std::list<int> values;
    for (int i = -1; i >= -15; i--) {
        golomb.encode(i);
        values.push_back(i);
    }
    golomb.~Golomb();

    BitStream reader('r', TEST_FILE_NAME);
    Golomb golomb2(5, reader, VALUE_INTERLEAVING);
    for (int i = -1; i >= -15; i--) {
        int decoded = golomb2.decode();
        int original = values.front();

        values.pop_front();
        EXPECT_EQ(decoded, original);
    }
    EXPECT_EQ(values.size(), 0);

    golomb2.~Golomb();

    remove(TEST_FILE_NAME);
};

TEST(Golomb, signMagnitude_intensive) {

    BitStream writer('w', TEST_FILE_NAME);
    Golomb golomb(11, writer, SIGN_MAGNITUDE);
    std::list<int> values;
    for (int i = -32764; i <= 32764; i++) {
        golomb.encode(i);
        values.push_back(i);
    }
    golomb.~Golomb();

    BitStream reader('r', TEST_FILE_NAME);
    Golomb golomb2(100, reader, SIGN_MAGNITUDE);
    for (int i = -32764; i <= 32764; i++) {
        int decoded = golomb2.decode();
        int original = values.front();

        values.pop_front();
        EXPECT_EQ(decoded, original);
    }
    EXPECT_EQ(values.size(), 0);

    golomb2.~Golomb();

    remove(TEST_FILE_NAME);
};

TEST(Golomb, valueInterleaving_intensive) {

    BitStream writer('w', TEST_FILE_NAME);
    Golomb golomb(11, writer, VALUE_INTERLEAVING);
    std::list<int> values;
    for (int i = -32764; i <= 32764; i++) {
        golomb.encode(i);
        values.push_back(i);
    }
    cout << "values size: " << values.size() << endl;
    golomb.~Golomb();

    BitStream reader('r', TEST_FILE_NAME);
    Golomb golomb2(100, reader, VALUE_INTERLEAVING);
    for (int i = -32764; i <= 32764; i++) {
        int decoded = golomb2.decode();
        int original = values.front();

        values.pop_front();
        EXPECT_EQ(decoded, original);
    }
    EXPECT_EQ(values.size(), 0);

    golomb2.~Golomb();

    remove(TEST_FILE_NAME);
};