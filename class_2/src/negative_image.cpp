#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// Without using possible existing functions of OpenCV
// Create the negative version of an image
int main(int argc, char** argv )
{
    if (argc < 3)
    {
        cerr << "Usage: " << argv[0] << " <input file> <output file>\n";
        return 1;
    }

    Mat img = imread(argv[1]);
    if(img.empty()){
        cerr << "Error: could not open/find image\n";  
        return 1;
    }

    // Percorrer cada pixel da imagem e inverter o valor
    for (int i = 0; i < img.rows; i++)
    {
        for (int j = 0; j < img.cols * img.channels(); j++)
        {
            img.at<uchar>(i, j) = 255 - img.at<uchar>(i, j);
        }
    }

    imwrite(argv[2], img);

    return 0;
}