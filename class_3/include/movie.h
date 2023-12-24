#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <opencv2/opencv.hpp>

typedef struct{
    uint16_t chroma;
    uint16_t width;
    uint16_t height;
    uint16_t fps;
    int bytesPerFrame;
    int currentFrameIndex;
}HeaderParameters;

using namespace std;
using namespace cv;
//Usar Mat, criar mat com um plano, .ptr() para obter o ponteiro po buffer, .at para aceder a um pixel
//
class Movie
{
private:
    
    int movieIndex;
    HeaderParameters headerParameters;
public:
    Movie(){
        this->movieIndex = 0;
        this->headerParameters.currentFrameIndex = 0;
    }
    ~Movie();

    //Get the parameters from an y4m file's header
    void Movie::getHeaderParameters(std::fstream movie){}

    //Auxiliar function used in getHeaderParameters to get one specific parameter
    uint16_t Movie::getParameter(string line, size_t startPos, char parameterType){}

    //Returns one frame of the video/movie
    Mat Movie::readFrameFromMovie(std::fstream movie){}

    //Creates a movie with the frames
    FILE createMovie(vector<Mat> frames){

    }
};
