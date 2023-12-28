#include "frame.h"

Frame::Frame(Mat frame, int frameSize) {
    this->frame = frame;
    this->frameSize = frameSize;
}

uint8_t Frame::getPixel(int pixelIndex) {
    uint8_t pixel = frame.at<uint8_t>(pixelIndex);
    return pixel;
}

vector<uint8_t> Frame::getBlock(short nBlocks) {
    vector<uint8_t> block(nBlocks);

    block.resize(nBlocks);  // pad with 0s

    int nPixelBlock = (frameSize / nBlocks) + (frameSize % nBlocks);

    // fix this
    //for (size_t i = 0; i < frameSize; i++) {
    //    block.push_back(frame.at<uint8_t>(i));
    //}

    return block;
}
