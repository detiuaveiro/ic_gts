#include "golomb_codec.h"

int predict1(int a, int b, int c) {
    return 1 * a - 3 * b + c;
};

/*
##############################################################################
###################         Golomb Encoder Code            ###################
##############################################################################
*/

GEncoder::GEncoder(int m = -1, PREDICTOR pred = AUTOMATIC)
    : writer('w', WRITER_FILE_NAME), golomb(DEFAULT_GOLOMB_M, writer) {
    if (m < 0)
        m = calculate_m(DEFAULT_GOLOMB_M);

    this->golomb.setM(m);
    this->predictor = pred;
}

GEncoder::~GEncoder() {
    golomb.flush();
    golomb.~Golomb();
    writer.~BitStream();
}

int GEncoder::calculate_m(int n) {
    double res = 1 - (log((1 + n)) / log(n));
    return std::ceil(res);
}

/*
##############################################################################
###################         Golomb Decoder Code            ###################
##############################################################################
*/

GDecoder::GDecoder(/* args */) {}

GDecoder::~GDecoder() {}