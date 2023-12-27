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
    Mat* frame;
    short frameSize;
    uchar* framePtr;
public:
    Frame(Mat* frame, int frameSize){}

    uint8_t getPixel(int pixelIndex){}

    vector<uint8_t> getBlock(short blockSize){}

    Mat* getFrame() {return this->frame;}
};


