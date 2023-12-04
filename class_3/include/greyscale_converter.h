#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <fstream>

class GreyscaleConverter
{
private:
    std::fstream file;
public:
    GreyscaleConverter(std::string fileName){
        this->file.open(fileName, std::fstream::in | std::fstream::binary);
    }

    bool IsGreyScale(){
        bool isGrey = false;

        //DO ALGORITHM TO CHECK

        return isGrey;
    }

    void ColorScaleToGreyScale(){
        //Perform Color Scale change
    }
    ~GreyscaleConverter();
};

