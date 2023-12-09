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
    GrayscaleConverter(std::string fileName) {
        this->file.open(fileName, std::fstream::in | std::fstream::binary);
        this->img = imread(fileName);
    }

    bool IsGrayScale() {
        //For grayscale, pixels must have the same value
        for (int i = 0; i < img.rows; i++)
        {
            for (int j = 0; j < img.cols; j++)
            {
                Vec3b pixel = img.at<Vec3b>(i, j);
                if((pixel[0] != pixel[1]) || (pixel[1] != pixel[2])) return false;
            }
            
        }
        return true;
    }

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
