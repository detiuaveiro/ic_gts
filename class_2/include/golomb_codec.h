#pragma once
#include <golomb.h>
#include <string>

#define WRITER_FILE_NAME "outputStream.bin"
#define DEFAULT_GOLOMB_M 4

enum PREDICTOR {DEFAULT, AUTOMATIC };

/*
##############################################################################
###################        Golomb Encoder Class            ###################
##############################################################################
*/

class GEncoder {
   private:
    Golomb golomb;
    BitStream writer;
    PREDICTOR predictor;

   public:
    GEncoder();
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
