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
    short frameSize;

   public:
    // Maybe use move constructor
    Frame(Mat frame, int frameSize);

    uint8_t getPixel(int pixelIndex);

    vector<uint8_t> getBlock(short nBlocks);

    Mat& getFrame() { return this->frame; }
};
