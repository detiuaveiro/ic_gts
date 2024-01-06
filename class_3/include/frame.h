#pragma once

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class Frame {
   private:
   public:
    static uint8_t get_pixel(Mat& image, int pixelIndex);

    static std::vector<cv::Mat> get_blocks(Mat& image, int blockSize);
};
