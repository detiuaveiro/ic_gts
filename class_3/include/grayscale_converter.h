#pragma once

#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string.h>

using namespace cv;
using namespace std;

class GrayscaleConverter {
   private:
    std::fstream file;
    Mat img;

   public:
    GrayscaleConverter(std::string fileName);

    bool IsGrayScale();

    void ColorScaleToGrayScale();

    void SaveToFile(std::string outputFileName);

    ~GrayscaleConverter(){}
};
