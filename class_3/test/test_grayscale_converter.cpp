#include <gtest/gtest.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// Import of local libraries
#include <grayscale_converter.h>

const std::string TEST_IMAGE_PATH = "images/airplane.ppm";

TEST(GrayscaleConverter, IsNotGrayScale) {
    GrayscaleConverter grayscaleConverter(TEST_IMAGE_PATH);
    EXPECT_EQ(grayscaleConverter.IsGrayScale(), false);
};

TEST(GrayscaleConverter, IsGrayScale) {
    GrayscaleConverter grayscaleConverter(TEST_IMAGE_PATH);
    grayscaleConverter.ColorScaleToGrayScale();
    grayscaleConverter.SaveToFile("images/airplane_gray.pgm");

    GrayscaleConverter grayscaleConverterGray("images/airplane_gray.pgm");
    EXPECT_EQ(grayscaleConverterGray.IsGrayScale(), true);
};


