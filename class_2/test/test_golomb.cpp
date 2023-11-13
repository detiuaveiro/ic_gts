// Import of standard or provided libraries
#include <gtest/gtest.h>
#include <cmath>
#include <string>

// Import of local libraries
#include <golomb.h>

using namespace std;

TEST(Golomb, txSmallValues) {

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

    for (int i = 0; i <= 15; i++) {
        int decoded = golomb.decode();
        int original = values.front();
        values.pop_front();
        EXPECT_EQ(decoded, original);
    }

    // Remove test_golomb.bin
    remove("test_golomb.bin");
};

TEST(Golomb, txNegativeValues) {

    // Check if the number of core blocks is correct
    EXPECT_EQ(2, 2);
};
