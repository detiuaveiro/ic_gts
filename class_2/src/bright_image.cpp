#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv )
{
    if (argc < 4)
    {
        cerr << "Usage: " << argv[0] << " <input file> <bright-factor> <output file>\n";
        return 1;
    }

    Mat img = imread(argv[1]);
    if(img.empty()){
        cerr << "Error: could not open/find image\n";  
        return 1;
    }

    double brightFactor = stod(argv[2]);
    if(brightFactor < 0){
        cerr << "Error: bright factor should not be less than 0\n";
    }
    for (int i = 0; i < img.rows; i++)
    {
        for (int j = 0; j < img.cols; j++)
        {
            img.at<Vec3b>(i,j) *= brightFactor;
        }
        
    }
    

    imwrite(argv[3], img);

    return 0;
}