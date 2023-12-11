#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <opencv2/opencv.hpp>

class Movie
{
private:
    int movieIndex;
public:
    Movie(){
        this->movieIndex = 0;
    }
    ~Movie();

    //returnsn one frame of the video/movie
    void readMovie(std::fstream movie){}

    void getFrame(){}

    void createMovie(){}
};
