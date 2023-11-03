#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// Extract a color channel from an image, creating a single channel image with the result
// File names and channel number passed as command line args
int main(int argc, char** argv )
{
    if (argc < 3)
    {
        cerr << "Usage: " << argv[0] << " <input file> <channel number>\n";
        return 1;
    }

    Mat img = imread(argv[1]);
    if(img.empty()){
        cerr << "Error: could not open/find image\n";  
        return 1;
    }

    int channel = atoi(argv[2]);
    if (channel < 1 || channel > 3) {
        cerr << "Error: channel number must be between 1 and 3 (RGB)\n";
        return 1;
    }

    return 0;
}