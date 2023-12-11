#pragma once

#include <golomb.h>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <image_predictor.h>

#define WRITER_FILE_NAME "outputStream.bin"
#define DEFAULT_GOLOMB_M 4

#define BITS_BLOCK_SIZE 16
#define BITS_SAMPLE_RATE 16
#define BITS_N_FRAMES 32
#define BITS_N_CHANNELS 4
#define BITS_BIT_RATE 16
#define BITS_LOSSY 2
#define BITS_APPROACH 4
#define BITS_M 14
#define BITS_PHASE 2
#define BITS_PREDICTOR 2

using namespace std;

/*
##############################################################################
###################           Data Structures              ###################
##############################################################################
*/

struct Block {
    /* Header */
    uint16_t m;
    // frame type -> objeto da classe do Samu
    PREDICTOR_TYPE predictor;
    /* Data */
    vector<vector<u_int8_t>> data; // 2D vector of bytes -> each byte represents a y value of a pixel
};

struct File {
    /* Header */
    uint16_t blockSize;
    uint8_t nChannels;
    uint16_t sampleRate;
    uint32_t nFrames;
    uint16_t bitRate;
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

    std::string outputFileName;
    File fileStruct;

    std::vector<unsigned short> abs_value_vector(std::vector<short>& values);
    int calculate_m(std::vector<short>& values);
    Block process_block(std::vector<short>& block, int blockId, int nBlocks);

    void write_file_header();
    void write_file_block(Block& block, int blockId, int nBlocks);

   public:
    GEncoder(std::string outFileName, int m = -1,
             PREDICTOR_TYPE pred = AUTOMATIC);
    ~GEncoder();

    void encode_file(File file, std::vector<short>& inSamples, size_t nBlocks);

    void setFile(File file);

    // Stuff used for testing private members
    int test_calculate_m(std::vector<short>& values);
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

    std::vector<short> decode_block(Block& block);

    Block read_file_block(int blockId, int nBlocks);

   public:
    GDecoder(std::string inFileName);
    ~GDecoder();

    int read_file_header();
    void print_processing_information();
    std::vector<short> decode_file();

    File& get_file();
};