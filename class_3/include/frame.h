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
    Mat frame;
    int frameSize;
    short indexX = 0;
    short indexY = 0;

   public:
    // Maybe use move constructor
    Frame(Mat frame, int frameSize);

    uint8_t getPixel(int pixelIndex);

    vector<uint8_t> getBlock(int nBlocks);

    Mat& getFrame() { return this->frame; }
};
