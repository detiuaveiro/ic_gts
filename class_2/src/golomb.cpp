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
// int Golomb::calculate_remainder(int r) {
//     // log2(m) = number of bits to represent m
    
    
// }

int Golomb::encode_sign_magnitude(int value) {
    int quotient = value / m;
    int remainder = value % m;
    
    bool isNegative = (value < 0) ? true : false;
    value = abs(value);

    // creating the unary with as many 1s as the quotient
    unsigned int result = (1 << quotient) - 1;
    // Insert 0 to separate the quotient from the remainder
    result <<= 1;

    int b = ceil(log2(m));

    // m is power of 2
    if ((m & (m - 1)) == 0){
        result <<= b;  // add b - 1 zeros to the end
        result += remainder;  // add remainder
    }
    // m is not power of 2
    if (remainder < (pow(2, b) - m)){
        result  <<= b - 1;  // add b - 1 zeros to the end
        result += remainder;  // add remainder
    }else{
        remainder += pow(2, b) - m;  
        result <<= b;  // add b zeros to the end
        result += remainder;  // add remainder
    }

    //result += calculate_remainder(remainder);
    if (value == 0)
        return 0;  // dont add signal

    if (isNegative)
        result = (result << 1) + 1;  // add 1 to end
    else
        result <<= 1;  // add 0 to end

    return result;
}

int Golomb::encode_value_interleaving(int value) {
    int quotient = value / m;
    //int remainder = value % m;

    // if number is negative, multiply by 2 and subtract 1
    if (value < 0)
        value = 2 * abs(value) - 1;
    else
        value = 2 * value;

    // creating the unary with as many 1s as the quotient
    unsigned int result = (1 << quotient) - 1;

    // Insert 0 to separate the quotient from the remainder
    result <<= 1;

    //result += calculate_remainder(remainder);

    return result;
}

void Golomb::encode(int value) {
    if (m <= 0)
        throw std::invalid_argument("m must be positive");

    int quotient = value / m;
    std::cout << "input value: " << value << std::endl;

    int result = 0;
    if (approach == SIGN_MAGNITUDE)
        result = encode_sign_magnitude(value);
    else if (approach == VALUE_INTERLEAVING)
        result = encode_value_interleaving(value);
    else
        throw std::invalid_argument("Invalid approach");

    std::cout << "golomb result: " << result << std::endl;
    
    int bits_to_represent = 0;
    int aux = result;
    do {
        aux >>= 1;
        bits_to_represent++;
    } while (aux != 0);
    if (quotient == 0){
        bits_to_represent++;
    }

    std::cout << "NUMBER OF BITS TO REPRESENT: " << bits_to_represent << std::endl;
    std::cout << std::endl;
    bitStream.writeNBits(result, bits_to_represent);
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

    int b = ceil(log2(m));
    int values_divider = pow(2, b) - m;

    // read the binary part
    for (int i = 0; i < b - 1; i++) {
        int bit = bitStream.readBit();
        remainder = (remainder << 1) + bit;
    }

    int extraBit = 10;
    if (remainder >= values_divider) {
        remainder = (remainder << 1) + bitStream.readBit();
    }

    if (extraBit != 10) {
        remainder = remainder - values_divider;
    }

    value = quotient * m + remainder;

    if (value == 0) {
        return value;
    } else if (approach == 1) {
        // SIGN AND MAGNITUDE

        int sign = bitStream.readBit();  // Last bit is the sign
        if (sign == 1)
            value = -value;
        return value;

    } else {
        // VALUE INTERLEAVING

        if (value % 2 == 1) {  // if number is odd, add 1 and divide by 2
            value++;
            value /= 2;
            value = -value;
        } else {
            value /= 2;
        }
        return value;
    }
}