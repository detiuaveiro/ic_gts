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
}HeaderParameters;

class Movie
{
private:
    int movieIndex;
    HeaderParameters headerParameters;
public:
    Movie(){
        this->movieIndex = 0;
    }
    ~Movie();

    //get the parameters from an y4m file's header
    void getHeaderParameters(std::fstream movie){}

    //auxiliar function used in getHeaderParameters to get one specific parameter
    uint16_t getParameter(string line, size_t startPos, char parameterType){}

    //returns one frame of the video/movie
    vector<uint8_t> readFrameFromMovie(std::fstream movie){}

    void createMovie(){}
};
