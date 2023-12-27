#include "frame.h"


Frame::Frame(Mat* frame, int frameSize){
    this->frame = frame;
    this->frameSize = frameSize;
    this->framePtr = (*this->frame).ptr();
}

uint8_t Frame::getPixel(int pixelIndex){
    cout << "Pixel: " << (*this->frame).at<uint8_t>(pixelIndex) << std::endl;
    return (*this->frame).at<uint8_t>(pixelIndex);
}

vector<uint8_t> Frame::getBlock(short blockSize){
    vector<uint8_t> block;

    for (size_t i = 0; i < frameSize; i++)
    {
        block.push_back((*this->frame).at<uint8_t>(i));
    }

    return block;
}

