#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <opencv2/opencv.hpp>

class Movie
{
private:
    int movieIndex;
    vector<uint16_t> headerParameters;
public:
    Movie(){
        this->movieIndex = 0;
    }
    ~Movie();

    void getHeaderParameters(std::fstream movie){}

    uint16_t getParameter(string line, size_t startPos){}

    //returns one frame of the video/movie
    vector<uint8_t> readFrameFromMovie(std::fstream movie){}

    void getFrame(){}

    void createMovie(){}
};
