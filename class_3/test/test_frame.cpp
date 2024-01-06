#include <frame.h>
#include <gtest/gtest.h>
#include <movie.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

/*
TEST(Frame, testGetPixel) {
    ASSERT_FALSE(img.empty()) << "Failed to read image file";
    Frame frame = Frame(img, 262144);

    ASSERT_FALSE(frame.getFrame().empty())
        << "Failed to copy image data to Frame object";

    
    cout << "Pixel: " << img.at<uint8_t>(0) << std::endl;
    cout << "Pixel: " << img.at<uint8_t>(262143) << std::endl;
    cout << "Rows: " << frame.getFrame().rows << std::endl;
    cout << "Cols: " << frame.getFrame().cols << std::endl;

    cout << "Pixel: " << frame.getFrame().at<uint8_t>(0) << std::endl;
    cout << "Pixel: " << frame.getFrame().at<uint8_t>(262143) << std::endl;

    uint8_t expectedPixelValue = img.at<uint8_t>(0);
    uint8_t expectedPixelValue2 = img.at<uint8_t>(262143);
    EXPECT_EQ(frame.getPixel(0), expectedPixelValue);
    EXPECT_EQ(frame.getPixel(262143), expectedPixelValue2);
}
*/

TEST(Frame, testGetBlock_image) {
    Mat img = imread("../images/airplane.ppm");

    vector<Mat> blocks = Frame::get_blocks(img, 16);

    for (Mat& block: blocks) {
        EXPECT_EQ(block.rows, 16);
        EXPECT_EQ(block.cols, 16);
    }

    int expectedBlockCount = img.rows / 16 * img.cols / 16;

    EXPECT_EQ(blocks.size(), expectedBlockCount);
}

TEST(Frame, testGetBlock_frame) {
    fstream movieStream;
    movieStream.open("../movies/sintel_trailer_2k_480p24.y4m",
                     std::fstream::in | std::fstream::binary);

    Movie movieClass = Movie();
    movieClass.getHeaderParameters(movieStream);

    EXPECT_EQ(movieClass.getChroma(), 420);
    EXPECT_EQ(movieClass.getWidth(), 854);
    EXPECT_EQ(movieClass.getHeight(), 480);
    EXPECT_EQ(movieClass.getFps(), "24:1");

    Mat img = movieClass.readFrameFromMovie(movieStream);

    vector<Mat> blocks = Frame::get_blocks(img, 16);

    for (Mat& block: blocks) {
        EXPECT_EQ(block.rows, 16);
        EXPECT_EQ(block.cols, 16);
    }

    int expectedBlockCount = img.rows / 16 * img.cols / 16;

    EXPECT_EQ(blocks.size(), expectedBlockCount);

    movieStream.close();
}

TEST(Movie, test) {
    fstream movieStream;
    movieStream.open("../movies/sintel_trailer_2k_480p24.y4m",
                     std::fstream::in | std::fstream::binary);

    Movie movieClass = Movie();
    movieClass.getHeaderParameters(movieStream);

    EXPECT_EQ(movieClass.getChroma(), 420);
    EXPECT_EQ(movieClass.getWidth(), 854);
    EXPECT_EQ(movieClass.getHeight(), 480);
    EXPECT_EQ(movieClass.getFps(), "24:1");

    Mat frame = movieClass.readFrameFromMovie(movieStream);
    EXPECT_NE(frame.size(), Mat().size());

    EXPECT_EQ(frame.size(), Size(854, 480));

    movieStream.close();
}
