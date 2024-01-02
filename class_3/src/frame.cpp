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

vector<uint8_t> Frame::getBlock(int nBlocks) {
    vector<uint8_t> block;

    /*
    std::cout << "blocks: " << nBlocks << std::endl;
    std::cout << "Frame Size: " << this->frameSize << std::endl;*/

    //number of pixels per block
    int nPixelBlock = (this->frameSize / nBlocks) + (this->frameSize % nBlocks);
    //Length of the edge of the block
    int blockEdge =  sqrt(nPixelBlock);

    /*
    std::cout << "blockEdge: " << blockEdge << std::endl;
    std::cout << "frameEdge: " << frameEdge << std::endl;
    std::cout << "nPixelBlock: " << nPixelBlock << std::endl;

    //Process blocks (literal geometrical squares)
    std::cout << indexY+blockEdge << std::endl;
    std::cout << indexX+blockEdge << std::endl;*/

    for (int i = indexY; i < indexY + blockEdge; i++)
    {
        for (int j = indexX; j < indexX + blockEdge; j++)
        {
            block.push_back(frame.at<uint8_t>(j, i));
        }
    }

    //if last block of a row is reached
    if(indexX + blockEdge == 512){
        indexX = 0;
        indexY +=blockEdge;
    }
    else indexX+=blockEdge;
    
    return block;
}
