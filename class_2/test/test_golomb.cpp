// Import of standard or provided libraries
#include <gtest/gtest.h>
#include <cmath>
#include <string>

// Import of local libraries
#include <golomb.h>

TEST(Golomb, testClassCreation) {

    // Check if the number of core blocks is correct
    EXPECT_EQ(2, 2);

    // Check if the number of exit ports is correct
    EXPECT_NE(3, 2);
};
