// Import of standard or provided libraries
#include <gtest/gtest.h>
#include <cmath>
#include <string>

// Import of local libraries
#include <golomb.h>

using namespace std;

TEST(Golomb, txSmallValues_mPowerOf2) {

    // Check if the number of core blocks is correct
    EXPECT_EQ(2, 2);

    // Check if the number of exit ports is correct
    EXPECT_NE(3, 2);

    BitStream writer('w', "test_golomb.bin");
    Golomb golomb(4, writer);
    std::list<int> values;
    for (int i = 0; i <= 20; i++) {
        golomb.encode(i);
        values.push_back(i);
    }
    golomb.~Golomb();

    BitStream reader('r', "test_golomb.bin");
    Golomb golomb2(4, reader);
    for (int i = 0; i <= 20; i++) {
        int decoded = golomb2.decode();
        int original = values.front();
        // cout << "decoded: " << decoded << endl;
        // cout << "original: " << original << endl;
        // cout << endl;
        values.pop_front();
        EXPECT_EQ(decoded, original);
    }
    golomb2.~Golomb();
};

TEST(Golomb, txSmallValues_mNotPowerOf2) {

    // Check if the number of core blocks is correct
    EXPECT_EQ(2, 2);

    // Check if the number of exit ports is correct
    EXPECT_NE(3, 2);

    BitStream writer('w', "test_golomb.bin");
    Golomb golomb(5, writer, 1);
    std::list<int> values;
    for (int i = 0; i <= 15; i++) {
        golomb.encode(i);
        values.push_back(i);
    }
    golomb.~Golomb();

    BitStream reader('r', "test_golomb.bin");
    Golomb golomb2(5, reader);
    for (int i = 0; i <= 15; i++) {
        int decoded = golomb2.decode();
        int original = values.front();
        // cout << "decoded: " << decoded << endl;
        // cout << "original: " << original << endl;
        // cout << endl;
        values.pop_front();
        EXPECT_EQ(decoded, original);
    }
    golomb2.~Golomb();
};

TEST(Golomb, txNegativeSignMagnitude) {

    // Check if the number of core blocks is correct
    EXPECT_EQ(2, 2);

    BitStream writer('w', "test_golomb.bin");
    Golomb golomb(11, writer);
    std::list<int> values;
    for (int i = 0; i >= -15; i--) {
        golomb.encode(i);
        values.push_back(i);
    }
    golomb.~Golomb();

    BitStream reader('r', "test_golomb.bin");
    Golomb golomb2(11, reader);
    for (int i = 0; i >= -15; i--) {
        int decoded = golomb2.decode();
        int original = values.front();
        // cout << "decoded: " << decoded << endl;
        // cout << "original: " << original << endl;
        // cout << endl;
        values.pop_front();
        EXPECT_EQ(decoded, original);
    }
    golomb2.~Golomb();
};

TEST(Golomb, txNegativeValueInterleaving) {

    // Check if the number of core blocks is correct
    EXPECT_EQ(2, 2);

    BitStream writer('w', "test_golomb.bin");
    Golomb golomb(5, writer, 2);
    std::list<int> values;
    for (int i = 0; i >= -15; i--) {
        golomb.encode(i);
        values.push_back(i);
    }
    golomb.~Golomb();

    BitStream reader('r', "test_golomb.bin");
    Golomb golomb2(5, reader, 2);
    for (int i = 0; i >= -15; i--) {
        int decoded = golomb2.decode();
        int original = values.front();
        // cout << "decoded: " << decoded << endl;
        // cout << "original: " << original << endl;
        // cout << endl;
        values.pop_front();
        EXPECT_EQ(decoded, original);
    }
    golomb2.~Golomb();
};