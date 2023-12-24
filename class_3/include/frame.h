#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class Frame
{
private:
    Mat frame;
    short frameSize;
    uchar* framePtr;
public:
    Frame::Frame(Mat frame, short frameSize){}

    uint8_t Frame::getPixel(short pixelIndex){}

    vector<uint8_t> Frame::getBlock(short blockSize){}
};


