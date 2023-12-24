#include "frame.h"


Frame::Frame(Mat frame, short frameSize){
    this->frame = frame;
    this->frameSize = frameSize;
    this->framePtr = frame.ptr();
}

uint8_t Frame::getPixel(short pixelIndex){
    return frame.at<uint8_t>(pixelIndex);
}

vector<uint8_t> Frame::getBlock(short blockSize){
    vector<uint8_t> block;

    for (size_t i = 0; i < frameSize; i++)
    {
        block.push_back(frame.at<uint8_t>(i));
    }

    return block;
}

