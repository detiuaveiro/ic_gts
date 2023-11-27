#pragma once

#include <cmath>
#include <iostream>
#include <string>
#include "bit_stream.h"

enum APPROACH { SIGN_MAGNITUDE, VALUE_INTERLEAVING };

// Functions for:
// - Encoding an integer (generating the corresponding sequence of bits)
// - Decoding a sequence of bits (generating the corresponding integer)

// Parameter m to allow fitting the codes to the appropriate probability distribution
// m is the number of bits used to represent the remainder

class Golomb {
   private:
    int m;  // distribution
    APPROACH approach = SIGN_MAGNITUDE;
    BitStream& bitStream;

    int calculate_remainder(int r);
    int encode_sign_magnitude(int value);
    int encode_value_interleaving(int value);

   public:
    Golomb(int m, BitStream& bitStream, APPROACH approach = SIGN_MAGNITUDE);

    ~Golomb() { flush(); }

    void flush() { bitStream.~BitStream(); }

    void setM(int newM) { this->m = newM; }
    int getM() { return m; }

    void setApproach(APPROACH newApproach) { this->approach = newApproach; }
    int getApproach() { return approach; }

    // Inteiro i é representado por 2 números: q e r
    // q é a parte inteira da divisão de n por m (parte unária)
    // r é o resto da divisão de n por m (parte binária)
    void encode(int i);

    // Decode a sequence of bits
    int decode();
};