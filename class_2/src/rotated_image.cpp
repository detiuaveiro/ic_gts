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

    int rowsLength = angle % 180 ? img.rows: img.cols;
    int colsLength = angle % 180 ? img.cols: img.rows;

    Mat rotated = Mat::zeros(rowsLength, colsLength, img.type());

    if(angle % 360 == 0){
        //input image == output image so it does nothing mk
        rotated = img;
    }
    else if(angle % 270 == 0){
        // Espelhar verticalmente -> Troca valores das linhas
        for (int i = 0; i < img.rows-1; i++)
        {
            for (int j = 0; j < img.cols-1; j++)
            {
                rotated.at<Vec3b>(img.rows - j - 1,i) = img.at<Vec3b>(i, j);
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
                rotated.at<Vec3b>(img.rows - i - 1, img.cols - j - 1) = img.at<Vec3b>(i,j);
            }
        }
    }
    else{
        for (int i = 0; i < img.rows; i++)
        {
            for (int j = 0; j < img.cols; j++)
            {
                rotated.at<Vec3b>(j, img.rows - i -1) = img.at<Vec3b>(i, j);
            }
            
        }
        
    }


    imwrite(argv[3], rotated);

    return 0;
}