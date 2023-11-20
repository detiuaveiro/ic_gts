#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv )
{
    if (argc < 4)
    {
        cerr << "Usage: " << argv[0] << " <input file> <multiple> <output file>\n";
        return 1;
    }

    Mat img = imread(argv[1]);
    if(img.empty()){
        cerr << "Error: could not open/find image\n";  
        return 1;
    }

    int angle = atoi(argv[2]) * 90;

    if(angle % 360 == 0){
        //input image == output image so it does nothing mk
    }
    else if(angle % 270 == 0){
        // Espelhar verticalmente -> Troca valores das linhas
        for (int i = 0; i < img.rows / 2; i++)
        {
            for (int j = 0; j < img.cols; j++)
            {
                Vec3b aux = img.at<Vec3b>(i, j);
                img.at<Vec3b>(i, j) = img.at<Vec3b>(i, img.cols - j - 1);
                img.at<Vec3b>(img.rows - i - 1, j) = aux;
            }
        }
    }
    else if (angle % 180 == 0){
        // Espelhar horizontalmente -> Troca valores das colunas
        for (int i = 0; i < img.rows; i++)
        {
            for (int j = 0; j < img.cols; j++)
            {
                //The first rows are equal to the inverse of the original last rows (i.e. row[0] = -(row[n]) )
                img.at<Vec3b>(i, j) = img.at<Vec3b>(img.rows-i-1, img.cols - j - 1);
            }
        }
    }
    else{
        
    }


    imwrite(argv[3], img);

    return 0;
}