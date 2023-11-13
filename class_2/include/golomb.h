#include <cmath>
#include <iostream>
#include <string>
#include "bit_stream.h"

// Functions for:
// - Encoding an integer (generating the corresponding sequence of bits)
// - Decoding a sequence of bits (generating the corresponding integer)

// Parameter m to allow fitting the codes to the appropriate probability distribution
// m is the number of bits used to represent the remainder

class Golomb {
   private:
    int m;         // distribution
    int approach;  // (1) sign magnitude (2) positive/negative value interleaving
    BitStream& bitStream;

    // Representar r de forma binária
    std::string getRemainderBinary(int r);

   public:
    Golomb(int m, BitStream& bitStream, int approach = 1);

    ~Golomb() { flush(); }

    void flush() { bitStream.~BitStream(); }

    // Inteiro i é representado por 2 números: q e r
    // q é a parte inteira da divisão de n por m (parte unária)
    // r é o resto da divisão de n por m (parte binária)
    void encode(int i);

    // Decode a sequence of bits
    int decode();
};