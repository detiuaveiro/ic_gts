#include "golomb.h"

Golomb::Golomb(int m, BitStream& bitStream, APPROACH approach)
    : bitStream(bitStream) {
    this->m = m;
    this->approach = approach;
}

bool check_approach(APPROACH approach){
    if(approach != SIGN_MAGNITUDE && approach != VALUE_INTERLEAVING)
        return false;
    return true; 
}

std::string approach_to_string(APPROACH approach){
    if(approach == SIGN_MAGNITUDE)
        return "SIGN_MAGNITUDE (0)";
    else if(approach == VALUE_INTERLEAVING)
        return "VALUE_INTERLEAVING (1)";
    else
        return "UNKNOWN";
}

// Represent remainder as binary
void Golomb::write_remainder(int remainder) {
    // m is power of 2
    int b = ceil(log2(m));

    if ((m & (m - 1)) == 0){
        bitStream.writeNBits(remainder, b);
    }else{
    // m is not power of 2
        if (remainder < (pow(2, b) - m)){
            bitStream.writeNBits(remainder, b-1);
        }else{
            remainder += pow(2, b) - m;
            bitStream.writeNBits(remainder, b);
        }
    }
}

void Golomb::encode_sign_magnitude(int value) {
    bool isNegative = (value < 0) ? true : false;
    value = abs(value);
    
    int quotient = value / m;
    int remainder = value % m;    

    // creating the unary with as many 1s as the quotient
    //unsigned int result = (1 << quotient) - 1;
    for(int i = 0; i < quotient; i++)
        bitStream.writeBit(1);

    // Insert 0 to separate the quotient from the remainder
    bitStream.writeBit(0);

    write_remainder(remainder);

    if (value == 0){ // 0 dont need signal
        return;
    }
    if (isNegative)
        bitStream.writeBit(1);  // add 1 to end
    else
        bitStream.writeBit(0);  // add 0 to end
}

void Golomb::encode_value_interleaving(int value) {
    // if number is negative, multiply by 2 and subtract 1
    if (value < 0)
        value = 2 * abs(value) - 1;
    else
        value = 2 * value;

    int quotient = value / m;
    int remainder = value % m;

    // creating the unary with as many 1s as the quotient
    for(int i = 0; i < quotient; i++)
        bitStream.writeBit(1);
    
    // Insert 0 to separate the quotient from the remainder
    bitStream.writeBit(0);

    write_remainder(remainder);
}

void Golomb::encode(int value) {
    if (m <= 0)
        throw std::invalid_argument("m must be positive");

    if (approach == SIGN_MAGNITUDE)
        encode_sign_magnitude(value);
    else if (approach == VALUE_INTERLEAVING)
        encode_value_interleaving(value);
    else
        throw std::invalid_argument("Invalid approach");
}

// Decode a sequence of bits
int Golomb::decode() {

    int value = 0;
    int quotient = 0;
    int remainder = 0;

    // read the unary part until a 0 is found
    while (true) {
        int bit = bitStream.readBit();
        if (bit == 0)
            break;
        quotient++;
    }

    int b = ceil(log2(m)); // 3
    int values_divider = pow(2, b) - m; // 3

    // read the binary part
    for (int i = 0; i < b - 1; i++) {
        int bit = bitStream.readBit();
        remainder = (remainder << 1) + bit;
    }

    int extraBit = 10;
    if (remainder >= values_divider) {
        remainder = (remainder << 1) + bitStream.readBit();
        extraBit = 1;
    }

    if (extraBit != 10) {
        remainder = remainder - values_divider;
    }

    value = quotient * m + remainder;

    if (value == 0) {   // no sign
        return value;
    } else if (approach == SIGN_MAGNITUDE) {

        int sign = bitStream.readBit();  // Last bit is the sign
        if (sign == 1)
            value = -value;
        return value;

    } else if (approach == VALUE_INTERLEAVING) {

        if (value % 2 == 1) {  // if number is odd, add 1 and divide by 2
            value++;
            value /= 2;
            value = -value;
        } else {
            value /= 2;
        }
        return value;
    } else
        std::invalid_argument("Invalid approach");
    return 0;
}