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
public:
    Frame(Mat frame){}

    uint8_t getPixel(){}

    vector<vector<uint8_t>> getBlock(){}
};


