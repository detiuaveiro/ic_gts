#include "frame.h"
#include <cmath>

Frame::Frame(Mat frame, int frameSize) {
    this->frame = frame;
    this->frameSize = frameSize;
}

uint8_t Frame::getPixel(int pixelIndex) {
    uint8_t pixel = frame.at<uint8_t>(pixelIndex);
    return pixel;
}

vector<uint8_t> Frame::getBlock(short nBlocks) {
    vector<uint8_t> block;

    //number of pixels per block
    int nPixelBlock = (frameSize / nBlocks) + (frameSize % nBlocks);
    //Length of the edge of the block
    short blockEdge = (short) sqrt(nPixelBlock);
    //Length of the edge of the frame
    short frameEdge = (short) sqrt(frameSize);

    //Process blocks (literal geometrical squares)
    for (short i = indexY; i < indexY + blockEdge; i++)
    {
        for (short j = indexX; j < indexX + blockEdge; j++)
        {
            block.push_back(frame.at<uint8_t>(i, j));
        }
    }
    //if last block of a row is reached
    if(indexX == frameEdge - blockEdge){
        indexX = 0;
        indexY +=blockEdge;
    }
    else indexX+=blockEdge;
    
    return block;
}
