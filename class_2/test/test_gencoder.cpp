// Import of standard or provided libraries
#include <gtest/gtest.h>
#include <cmath>
#include <string>

// Import of local libraries
#include <golomb_codec.h>

#define TEST_FILE_NAME "test_golomb_encoder.bin"

using namespace std;

TEST(GEncoder, testCalculateM) {
    GEncoder gEncoder(TEST_FILE_NAME, 5, AUTOMATIC);

    std::vector<short> samples = {1, 2, 3, 4, 5};
    int m = gEncoder.test_calculate_m(samples, samples.size());

    // complete this
};