#pragma once

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

typedef struct {
    uint16_t chroma;
    uint16_t width;
    uint16_t height;
    std::string fps;
    int bytesPerFrame;
    int currentFrameIndex;
    int numberFrames;
} HeaderParameters;

using namespace std;
using namespace cv;

//Usar Mat, criar mat com um plano, .ptr() para obter o ponteiro po buffer, .at para aceder a um pixel
//
class Movie {
   private:
    int movieIndex;
    HeaderParameters headerParameters;

   public:
    Movie() {
        this->movieIndex = 0;
        this->headerParameters.currentFrameIndex = 0;
    }

    //Get the parameters from an y4m file's header
    void getHeaderParameters(std::fstream& movie);

    //Auxiliar function used in getHeaderParameters to get one specific parameter
    string getParameter(string line, size_t startPos, char parameterType);

    //Returns one frame of the video/movie
    Mat readFrameFromMovie(std::fstream& movie);

    //Creates a movie with the frames
    FILE createMovie(vector<Mat> frames);

    uint16_t getChroma() { return this->headerParameters.chroma; }

    uint16_t getWidth() { return this->headerParameters.width; }

    uint16_t getHeight() { return this->headerParameters.height; }

    string getFps() { return this->headerParameters.fps; }

    int getBytesPerFrame() { return this->headerParameters.bytesPerFrame; }

    int getNumberFrames() { return this->headerParameters.numberFrames; }

    int getCurrentFrameIndex() {
        return this->headerParameters.currentFrameIndex;
    }
};
