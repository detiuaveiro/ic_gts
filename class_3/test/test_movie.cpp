#include <gtest/gtest.h>
#include <movie.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

TEST(Movie, testGetHeaderParameters) {
    fstream movieStream;
    movieStream.open("../movies/sintel_trailer_2k_480p24.y4m",
                     std::fstream::in | std::fstream::binary);

    Movie movieClass = Movie();
    movieClass.get_header_parameters(movieStream);

    EXPECT_EQ(movieClass.get_format(), "YUV4MPEG2");
    EXPECT_EQ(movieClass.get_chroma(), "C420jpeg");
    EXPECT_EQ(movieClass.get_width(), 854);
    EXPECT_EQ(movieClass.get_height(), 480);
    EXPECT_EQ(movieClass.get_fps(), 24);
    EXPECT_EQ(movieClass.get_interlace(), "Ip");
    EXPECT_EQ(movieClass.get_aspectRatio(), "A1:1");
    EXPECT_EQ(movieClass.get_fileSize(), 770452201);
    EXPECT_EQ(movieClass.get_frame_size(), 1229760);
    EXPECT_EQ(movieClass.get_number_frames(), 626);

    movieStream.close();
}

TEST(Movie, testReadFrameFromMovie) {
    fstream movieStream;
    movieStream.open("../movies/sintel_trailer_2k_480p24.y4m",
                     std::fstream::in | std::fstream::binary);

    Movie movieClass = Movie();
    movieClass.get_header_parameters(movieStream);

    EXPECT_EQ(movieClass.get_width(), 854);
    EXPECT_EQ(movieClass.get_height(), 480);
    EXPECT_EQ(movieClass.get_fps(), 24);

    Mat mat = Mat();
    int frameCounter = 1;
    while (true) {
        Mat frame = movieClass.read_frame(movieStream);
        if (frame.size() == mat.size())
            break;
        frameCounter++;
    }

    EXPECT_EQ(movieClass.get_number_frames(), frameCounter);

    movieStream.close();
}
