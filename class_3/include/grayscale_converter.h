#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class GrayscaleConverter {
   private:
    std::fstream file;
    Mat img;

   public:
    GrayscaleConverter(std::string fileName) {}

    bool IsGrayScale() {}

    void ColorScaleToGrayScale() {
        //Perform Color Scale change
        if (IsGrayScale()) {
            cout << "Image is already in greyscale\n";
            return;
        }
        cvtColor(img, img, COLOR_BGR2GRAY);
    }

    void SaveToFile(std::string outputFileName) {
        imwrite(outputFileName, img);
        cout << "Grayscale image saved to: " << outputFileName << endl;
    }
    ~GrayscaleConverter(){}
};
