#include <gtest/gtest.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <opencv2/opencv.hpp>
#include <frame.h>

using namespace cv;
using namespace std;

Mat img = imread("../images/airplane.ppm");
TEST(Frame, testGetPixel){
    Frame frame = Frame(img, 512*512);

    uint8_t expectedPixelValue = img.at<uint8_t>(0);
    uint8_t expectedPixelValue2 = img.at<uint8_t>(512);
    EXPECT_EQ(frame.getPixel(0), expectedPixelValue);
    EXPECT_EQ(frame.getPixel(512), expectedPixelValue2);
}

TEST(Frame, testGetBlock){
    Frame frame = Frame(img, 512*512);

    vector<uint8_t> block = frame.getBlock(16);
    EXPECT_EQ(block.size(), 16);
}
