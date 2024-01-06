#include <gtest/gtest.h>
#include <golomb.h>
//#include <bitstream.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

#define TEST_FILE_NAME "test_golomb.bin"

using namespace cv;
using namespace std;


TEST(Golomb, totalBits_necessary) {

    // test from 0 to 15
    // must return a total of 3+4+4+5+5+5+5+5+6+6+6+6+6+7+7+7 = 87

    int m = 5;
    BitStream bitStream('w', TEST_FILE_NAME);

    Golomb golomb(m,bitStream);

    std::vector<int> values = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    int bits = golomb.get_bits_needed(values);

    EXPECT_EQ(bits, 87);
};