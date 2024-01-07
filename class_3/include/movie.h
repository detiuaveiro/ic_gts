#pragma once

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <regex>

struct HeaderParameters {
    std::string format;
    std::string chroma;
    uint16_t width;
    uint16_t height;
    uint16_t fps;
    std::string interlace;
    std::string aspectRatio;
    unsigned long fileSize;
    int numberFrames;
    double frameSize;
};

using namespace std;
using namespace cv;

//Usar Mat, criar mat com um plano, .ptr() para obter o ponteiro po buffer, .at para aceder a um pixel
//
class Movie {
   private:
    HeaderParameters headerParameters;

    bool check_contains_frame(string line);

   public:
    Movie() {}

    //Get the parameters from an y4m file's header
    HeaderParameters get_header_parameters(std::fstream& stream);

    //Returns one frame of the video/movie
    Mat read_frame(std::fstream& stream);

    //Creates a movie with the frames
    FILE createMovie(vector<Mat> frames);

    string get_format() { return this->headerParameters.format; }

    string get_chroma() { return this->headerParameters.chroma; }

    uint16_t get_width() { return this->headerParameters.width; }

    uint16_t get_height() { return this->headerParameters.height; }

    uint16_t get_fps() { return this->headerParameters.fps; }

    string get_interlace() { return this->headerParameters.interlace; }

    string get_aspectRatio() { return this->headerParameters.aspectRatio; }

    unsigned long get_fileSize() { return this->headerParameters.fileSize; }

    int get_number_frames() { return this->headerParameters.numberFrames; }

    int get_frame_size() { return this->headerParameters.frameSize; };
};