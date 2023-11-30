// Import of standard or provided libraries
#include <gtest/gtest.h>
#include <cmath>
#include <string>

// Import of local libraries
#include <golomb_codec.h>

#define TEST_FILE_NAME "test_golomb_encoder.bin"

using namespace std;

TEST(GEncoder, testCalculateM) {
    GEncoder gEncoder(TEST_FILE_NAME, 5, AUTOMATIC, NO_CORRELATION);

    std::vector<short> samples = {1, 2, 3, 4, 5};
    int m = gEncoder.test_calculate_m(samples);

    EXPECT_EQ(m, 3);
};

TEST(GEncoder, testAbsValues) {
    GEncoder gEncoder(TEST_FILE_NAME, 5, AUTOMATIC, NO_CORRELATION);

    std::vector<short> samples = {-1, 2, -3, 4, -5};
    std::vector<unsigned short> abs_values =
        gEncoder.test_abs_value_vector(samples);

    EXPECT_EQ(samples.size(), abs_values.size());

    for (int i = 0; i < (int)samples.size(); i++) {
        EXPECT_EQ(abs(samples[i]), abs_values[i]);
    }
};