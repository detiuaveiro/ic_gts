#pragma once

#include <golomb.h>
#include <string>

#define WRITER_FILE_NAME "outputStream.bin"
#define DEFAULT_GOLOMB_M 4

using namespace std;

enum PREDICTOR { DEFAULT, AUTOMATIC, PREDICT1 };

struct Block {
    PREDICTOR predictor;
    vector<int> data;
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

    int calculate_m(int n);

    //void make_file_header();
    //void make_block_header(Block block);

   public:
    GEncoder(int m, PREDICTOR pred);
    ~GEncoder();
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
