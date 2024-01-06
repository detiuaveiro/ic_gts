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
    movieClass.getHeaderParameters(movieStream);

    EXPECT_EQ(movieClass.getChroma(), 420);
    EXPECT_EQ(movieClass.getWidth(), 854);
    EXPECT_EQ(movieClass.getHeight(), 480);
    EXPECT_EQ(movieClass.getFps(), "24:1");

    movieStream.close();
}

TEST(Movie, testReadFrameFromMovie) {
    fstream movie;
    movie.open("../movies/sintel_trailer_2k_480p24.y4m",
               std::fstream::in | std::fstream::binary);
    Movie movieObj = Movie();
    movieObj.getHeaderParameters(movie);
    Mat mat = Mat();
    while (true) {
        Mat frame = movieObj.readFrameFromMovie(movie);
        if (frame.size() == mat.size())
            break;
        //std::cout << "FRAME SIZE: \n" << frame.size();
    }
    /*

    for (size_t i = 0; i < 10; i++)
    {
        Mat frame = movieObj.readFrameFromMovie(movie);
        std::cout << "FRAME SIZE: \n" << frame.size();
    }
    */
}

TEST(Movie, testIntensive) {
    fstream movieStream;
    movieStream.open("../movies/sintel_trailer_2k_480p24.y4m",
                     std::fstream::in | std::fstream::binary);

    Movie movieClass = Movie();
    movieClass.getHeaderParameters(movieStream);

    EXPECT_EQ(movieClass.getChroma(), 420);
    EXPECT_EQ(movieClass.getWidth(), 854);
    EXPECT_EQ(movieClass.getHeight(), 480);
    EXPECT_EQ(movieClass.getFps(), "24:1");

    Mat mat = Mat();
    int frameCounter = 0;
    while (true) {
        Mat frame = movieClass.readFrameFromMovie(movieStream);
        if(frame.size() == mat.size()) break;
        frameCounter++;
    }

    EXPECT_EQ(movieClass.getNumberFrames(), frameCounter);

    movieStream.close();
}