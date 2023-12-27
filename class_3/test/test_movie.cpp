#include <gtest/gtest.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <opencv2/opencv.hpp>
#include <movie.h>

using namespace cv;
using namespace std;

TEST(Movie, testGetHeaderParameters){
    fstream movie;
    movie.open("../movies/sintel_trailer_2k_480p24.y4m", std::fstream::in | std::fstream::binary);

    Movie movieObj = Movie();
    movieObj.getHeaderParameters(movie);

    EXPECT_EQ(movieObj.getChroma(), 420);
    EXPECT_EQ(movieObj.getWidth(), 854);
    EXPECT_EQ(movieObj.getHeight(), 480);
    EXPECT_EQ(movieObj.getFps(), "24:1");

    movie.close();
}