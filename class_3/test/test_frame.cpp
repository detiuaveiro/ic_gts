#include <frame.h>
#include <gtest/gtest.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat img = imread("../images/airplane.ppm");

TEST(Frame, testGetPixel) {
    cout << "Reached" << std::endl;
    ASSERT_FALSE(img.empty()) << "Failed to read image file";
    Frame frame = Frame(img, 262144);
    cout << "Reached" << std::endl;

    ASSERT_FALSE(frame.getFrame().empty())
        << "Failed to copy image data to Frame object";

    cout << "Pixel: " << img.at<uint8_t>(0) << std::endl;
    cout << "Pixel: " << img.at<uint8_t>(512) << std::endl;
    cout << "Rows: " << frame.getFrame().rows << std::endl;
    cout << "Cols: " << frame.getFrame().cols << std::endl;

    cout << "Pixel: " << frame.getFrame().at<uint8_t>(0) << std::endl;
    cout << "Pixel: " << frame.getFrame().at<uint8_t>(512) << std::endl;

    uint8_t expectedPixelValue = img.at<uint8_t>(0);
    uint8_t expectedPixelValue2 = img.at<uint8_t>(512);
    EXPECT_EQ(frame.getPixel(0), expectedPixelValue);
    EXPECT_EQ(frame.getPixel(512), expectedPixelValue2);
}

TEST(Frame, testGetBlock) {
    Frame frame = Frame(img, 262144);

    vector<uint8_t> block = frame.getBlock(16);
    EXPECT_EQ(block.size(), 16);
}
