#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv )
{
    if (argc < 3)
    {
        cerr << "Usage: " << argv[0] << " <input file> <option> <output file>\n";
        return 1;
    }

    Mat img = imread(argv[1]);
    if(img.empty()){
        cerr << "Error: could not open/find image\n";  
        return 1;
    }

    string option = argv[2];
    if (option != "h" && option != "v")
    {
        cerr << "Error: invalid option, must be h (horizontally) or v (vertically)\n";
        return 1;
    }

    if (option == "h")
    {
        // Espelhar horizontalmente -> Troca valores das colunas
        for (int i = 0; i < img.rows; i++)
        {
            for (int j = 0; j < img.cols / 2; j++)
            {
                Vec3b aux = img.at<Vec3b>(i, j);    // Vec3b é um vetor de 3 bytes com os valores BGR
                img.at<Vec3b>(i, j) = img.at<Vec3b>(i, img.cols - j - 1);
                img.at<Vec3b>(i, img.cols - j - 1) = aux;
            }
        }
    }
    else
    {
        // Espelhar verticalmente -> Troca valores das linhas
        for (int i = 0; i < img.rows / 2; i++)
        {
            for (int j = 0; j < img.cols; j++)
            {
                Vec3b aux = img.at<Vec3b>(i, j);
                img.at<Vec3b>(i, j) = img.at<Vec3b>(img.rows - i - 1, j);
                img.at<Vec3b>(img.rows - i - 1, j) = aux;
            }
        }
    }

    imwrite(argv[3], img);

    return 0;
}