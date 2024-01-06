#pragma once

#include <golomb.h>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <image_predictor.h>
#include <opencv2/opencv.hpp>

#define WRITER_FILE_NAME "outputStream.bin"
#define DEFAULT_GOLOMB_M 4

// TODO - Mess with this values and see results
#define BITS_FILE_TYPE 1
#define BITS_BLOCK_SIZE 16
//#define BITS_N_FRAMES 32
#define BITS_CHROMA 16
#define BITS_WIDTH 16
#define BITS_HEIGHT 16
#define BITS_FPS 32 // F24:1 -> 24 fps (film), to represent 24:1 we need 32 bits
#define BITS_APPROACH 1 // sign magnitud, value interleaving, golomb
#define BITS_LOSSY 2

#define BITS_FRAME_TYPE 1 // for each block and frame

#define BITS_M 14
#define BITS_PREDICTOR 4

using namespace std;

enum FRAME_TYPE { I, P };

enum FILE_TYPE { Y4M, PGM };
/*
##############################################################################
###################           Data Structures              ###################
##############################################################################
*/

struct Block {
    /* Header */
    uint16_t m;
    FRAME_TYPE type; // I or P
    PREDICTOR_TYPE predictor;
    /* Data */
    vector<uint8_t> data; // 1D vector of bytes
};

struct FrameSegment {
    FRAME_TYPE type;
    vector<Block> blocks;
};

struct File {
    /* Header */
    FILE_TYPE type;
    uint16_t blockSize;
    //uint32_t nFrames;
    uint16_t chroma;
    uint16_t width;
    uint16_t height;
    string fps;
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
    //int frameId = 0;
    //Frame frame;

    std::string outputFileName;
    File fileStruct;

    // TODO - DELETE abs_value_vector?
    std::vector<unsigned short> abs_value_vector(std::vector<short>& values);
    int calculate_m(std::vector<uint8_t>& values);
    Block process_block(std::vector<uint8_t>& block, int blockId, int nBlocks);

    void write_file_header();
    // TODO
    void write_file_frame(FrameSegment& frame);
    void write_file_block(Block& block, int blockId, int nBlocks);

   public:
    GEncoder(std::string outFileName, int m = -1,
             PREDICTOR_TYPE pred = AUTOMATIC);
    ~GEncoder();

    // TODO
    // a main é que recebe o movie e chama a função encode_frame (EM VEZ DA ENCODE FILE!!!)
    void encode_file_header(File file);

    // TODO - Usar funções do Samu para fazer a funçao (buscar blocos, etc)
    void encode_frame(Frame frame, size_t nBlocks);

    void setFile(File file);

    // Stuff used for testing private members (TODO)
    int test_calculate_m(std::vector<std::vector<uint8_t>>& values);
    std::vector<unsigned short> test_abs_value_vector(
        std::vector<short>& values);
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

    bool headerRead = false;

    std::vector<uint8_t> decode_block(Block& block);

    Block read_file_block(int blockId, int nBlocks);

   public:
    GDecoder(std::string inFileName);
    ~GDecoder();

    int read_file_header();
    void print_processing_information();
    std::vector<short> decode_file();

    File& get_file();
};