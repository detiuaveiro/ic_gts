#pragma once

#include <golomb.h>
#include <iostream>
#include <string>
#include <tuple>

#define WRITER_FILE_NAME "outputStream.bin"
#define DEFAULT_GOLOMB_M 4

#define BITS_BLOCK_SIZE 16
#define BITS_SAMPLE_RATE 16
#define BITS_N_FRAMES 32
#define BITS_N_CHANNELS 4
#define BITS_BIT_RATE 8
#define BITS_LOSSY 2
#define BITS_APPROACH 4
#define BITS_M 14
#define BITS_PHASE 2
#define BITS_PREDICTOR 2

using namespace std;

/*
##############################################################################
###################           Predictor Class              ###################
##############################################################################
*/

enum PREDICTOR_TYPE { AUTOMATIC, PREDICT1, PREDICT2, PREDICT3 };

enum PHASE { P_AUTOMATIC, NO_CORRELATION, CORRELATION_MID, CORRELATION_SIDE };

std::string get_type_string(PREDICTOR_TYPE type);

std::string get_phase_string(PHASE phase);

class Predictor {
   private:
    /*! a1, a2 and a3 represent, a(n-1) a(n-2) and a(n-3) respectively
            where n is the index of the predicted sample */
    int predict1(int a1);
    int predict2(int a1, int a2);
    int predict3(int a1, int a2, int a3);

    double calculate_entropy(PREDICTOR_TYPE type, PHASE phase,
                             std::vector<short>& samples);

    int predict_no_correlation(PREDICTOR_TYPE type, std::vector<short>& samples,
                               int index);

    int predict_correlated_mid(PREDICTOR_TYPE type, std::vector<short>& samples,
                               int index);
    int predict_correlated_side(PREDICTOR_TYPE type,
                                std::vector<short>& samples, int index);

    int nChannels = 1;

   public:
    Predictor(int nChannels);
    Predictor();
    ~Predictor();

    /*! 
        Pass a set of samples/block and return the best predictor to be used 
            (the one that resulted in less occupied space)
    */
    std::tuple<PREDICTOR_TYPE, PHASE> benchmark(
        std::vector<short>& samples, PREDICTOR_TYPE bestPredictor = AUTOMATIC,
        PHASE bestPhase = P_AUTOMATIC);

    /*!
        Predict the next sample based on the type of the predictor and the
            previous samples
    */
    int predict(PREDICTOR_TYPE type, PHASE phase, std::vector<short>& samples,
                int index);

    bool check_type(PREDICTOR_TYPE type);
    bool check_phase(PHASE phase);
    void set_nChannels(int nChannels);
    int get_nChannels();
};

/*
##############################################################################
###################           Data Structures              ###################
##############################################################################
*/

struct Block {
    /* Header */
    uint16_t m;
    uint8_t phase;
    PREDICTOR_TYPE predictor;
    /* Data */
    vector<short> data;
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
    /* Data */
    vector<Block> blocks;
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
    PHASE phase = P_AUTOMATIC;
    int m;

    std::string outputFileName;
    File fileStruct;

    std::vector<unsigned short> abs_value_vector(std::vector<short>& values);
    int calculate_m(std::vector<short>& values);
    Block process_block(std::vector<short>& block, int blockId, int nBlocks, bool lossy);
    int lossy_error(int error, PREDICTOR_TYPE pred, int currentIndex, std::vector<short>& samples);

    void write_file();

   public:
    GEncoder(std::string outFileName, int m, PREDICTOR_TYPE pred, PHASE phase);
    ~GEncoder();

    void encode_file(File file, std::vector<short>& inSamples, size_t nBlocks);

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

    std::vector<short> decode_block(Block& block);

   public:
    GDecoder(std::string inFileName);
    ~GDecoder();

    File& read_file();
    std::vector<short> decode_file();
};