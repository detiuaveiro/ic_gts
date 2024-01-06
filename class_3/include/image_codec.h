#pragma once

#include <golomb.h>
#include <image_predictor.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <tuple>

#define WRITER_FILE_NAME "outputStream.bin"
#define DEFAULT_GOLOMB_M 4

#define BITS_FILE_TYPE 2
#define BITS_BLOCK_SIZE 16
#define BITS_N_FRAMES 32
#define BITS_CHROMA 16
#define BITS_WIDTH 16
#define BITS_HEIGHT 16
#define BITS_FPS 8
#define BITS_APPROACH 1  // sign magnitud, value interleaving, golomb
#define BITS_LOSSY 2

#define BITS_FRAME_TYPE 1  // for each block and frame

#define BITS_M 4
#define BITS_PREDICTOR 4

using namespace std;
using namespace cv;

enum FRAME_TYPE { I, P };

enum FILE_TYPE { Y4M, PGM };
/*
##############################################################################
###################           Data Structures              ###################
##############################################################################
*/

struct Block {
    /* Header */
    FRAME_TYPE type;  // I or P
    /* Data */
    std::vector<uint8_t> data;
};

struct FrameSegment {
    /* Header */
    FRAME_TYPE type;
    uint16_t m;
    PREDICTOR_TYPE predictor;
};

struct File {
    /* Header */
    FILE_TYPE type;
    uint16_t blockSize;
    uint32_t nFrames;
    uint16_t chroma;
    uint16_t width;
    uint16_t height;
    uint8_t fps;
    APPROACH approach;
    bool lossy;  // true if lossy, false if lossless
};

/*
##############################################################################
###################        Golomb Encoder Class            ###################
##############################################################################
*/

class GEncoder {
   private:
    BitStream writer;
    Golomb golomb;
    PREDICTOR_TYPE predictor = AUTOMATIC;
    Predictor predictorClass;
    int m;
    size_t nBlocksPerFrame;
    int intraFramePeriodicity;

    std::string outputFileName;
    File fileStruct;

    int calculate_m(Mat& values);
    Block process_block(Mat& block, int blockId, int nBlocks,
                        PREDICTOR_TYPE pred, FrameSegment& frame);

    void write_file_header();
    void write_frame_header(FrameSegment& frame);
    void write_file_block(Block& block, int blockId, int nBlocks,
                          FrameSegment& frame);

   public:
    GEncoder(std::string outFileName, int m = -1,
             PREDICTOR_TYPE pred = AUTOMATIC);
    ~GEncoder();

    void encode_file_header(File file, size_t nBlocksPerFrame,
                            int intraFramePeriodicity);

    void encode_frame(Mat frame, int frameId);

    void setFile(File file);

    // Stuff used for testing private members
    int test_calculate_m(Mat& values);
};

/*
##############################################################################
###################        Golomb Decoder Class            ###################
##############################################################################
*/

class GDecoder {
   private:
    BitStream reader;
    Golomb golomb;

    std::string inputFileName;
    File fileStruct;
    Predictor predictorClass;
    int nBlocksPerFrame;
    bool headerRead = false;

    FrameSegment read_frame_header();
    Block read_file_block(int blockId, int nBlocks, FrameSegment& frame);
    Mat decode_block(Mat& block, FrameSegment& frame);

   public:
    GDecoder(std::string inFileName);
    ~GDecoder();

    int read_file_header();
    Mat decode_frame(int frameId);

    File& get_file();
};