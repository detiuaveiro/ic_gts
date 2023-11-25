#pragma once

#include <golomb.h>
#include <string>

#define WRITER_FILE_NAME "outputStream.bin"
#define DEFAULT_GOLOMB_M 4
#define LEFT_MONO_CHANNEL 0
#define RIGHT_CHANNEL 1

using namespace std;

enum PREDICTOR { AUTOMATIC, PREDICT1 };

struct Block {
    /* Header */
    uint8_t m;
    PREDICTOR predictor;
    /* Data */
    vector<short> data;
};

struct File {
    /* Header */
    uint16_t blockSize;
    uint8_t nChannels;
    uint16_t sampleRate;
    uint32_t nFrames;
    uint16_t quantizationBits;
    /* Data */
    vector<Block> blocks;
};

int predict1(int a, int b, int c);

/*
##############################################################################
###################        Golomb Encoder Class            ###################
##############################################################################
*/

class GEncoder {
   private:
    BitStream writer;
    Golomb golomb;
    PREDICTOR predictor;
    int m;

    std::string outputFileName;
    File fileStruct;

    int calculate_m(std::vector<short>& values, int blockSize);
    Block process_block(std::vector<short>& block);
    void write_file();
    void quantize_samples(std::vector<short>& inSamples);

   public:
    GEncoder(std::string outFileName, int m, PREDICTOR pred);
    ~GEncoder();

    void encode_file(File file, std::vector<short>& inSamples, size_t nBlocks);
};

/*
##############################################################################
###################        Golomb Decoder Class            ###################
##############################################################################
*/

class GDecoder {
   private:
    /* data */
   public:
    GDecoder(/* args */);
    ~GDecoder();
};
