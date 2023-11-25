#include "golomb.h"

Golomb::Golomb(int m, BitStream& bitStream, int approach)
    : bitStream(bitStream) {
    this->m = m;
    this->approach = approach;
    if (approach != 1 && approach != 2) {
        throw std::invalid_argument(
            "Approach must be 1 - sign magnitude or 2 - positive/negative "
            "value interleaving");
    }
}

// Represent remainder as binary
std::string Golomb::getRemainderBinary(int r) {
    std::string golomb_remainder = "";

    if ((m & (m - 1)) == 0)  // m is power of 2
    {
        for (int i = 0; i < ceil(log2(m));
             i++) {  // log2(m) = number of bits to represent m
            golomb_remainder = std::to_string(r % 2) + golomb_remainder;
            r /= 2;
        }
    } else  // truncate when m is not power of 2
    {
        int b = ceil(log2(m));
        // encode first 2^b - m values of r using b - 1 bits
        if (r < pow(2, b) - m) {
            for (int i = 0; i < b - 1; i++) {
                golomb_remainder = std::to_string(r % 2) + golomb_remainder;
                r /= 2;
            }
        } else {  // encode the remaining values of r using b bits
            r += pow(2, b) - m;
            for (int i = 0; i < b; i++) {
                golomb_remainder = std::to_string(r % 2) + golomb_remainder;
                r /= 2;
            }
        }
    }

    return golomb_remainder;
}

void Golomb::encode(int value) {
    std::string golomb_code = "";
    int quocient = 0;
    int remainder = 0;
    if (approach ==
        1) {  // SIGN AND MAGNITUDE

        bool isNegative = false;
        if (value < 0) {
            isNegative = true;
            value = -value;  // Encode the positive value, the sign will be added later
        }

        quocient = floor(value / m);
        remainder = value % m;

        for (int j = 0; j < quocient; j++)
            golomb_code += "1";  // represent quocient as unary
        golomb_code += "0";      // separate quocient from remainder

        // represent remainder as binary
        golomb_code += getRemainderBinary(remainder);

        if (value != 0) {
            if (isNegative)
                golomb_code = golomb_code + "1";
            else
                golomb_code = golomb_code + "0";
        }
        // convert golomb_code (binary) to long int
        long long_val = 0;
        for (char c : golomb_code) {
            long_val = (long_val << 1) + (c - '0');
        }
        bitStream.writeNBits(long_val, golomb_code.length());

    } else {
        // VALUE INTERLEAVING

        // if number is negative, multiply by 2 and subtract 1
        if (value < 0)
            value = 2 * -value - 1;
        else
            value = 2 * value;

        quocient = floor(value / m);  
        remainder = value % m;

        for (int j = 0; j < quocient; j++)
            golomb_code += "1";  
        golomb_code += "0";      

        golomb_code += getRemainderBinary(remainder);

        long long_val = 0;
        for (char c : golomb_code) {
            long_val = (long_val << 1) + (c - '0');
        }
        bitStream.writeNBits(long_val, golomb_code.length());
    }
}

// Decode a sequence of bits
int Golomb::decode() {

    int value = 0;
    int quocient = 0;
    int remainder = 0;

    // read the unary part until a 0 is found
    std::string unary = "";
    while (true) {
        int bit = bitStream.readBit();
        if (bit == 0)
            break;
        quocient++;
    }

    int b = ceil(log2(m));
    int first_values = pow(2, b) - m;

    // read the binary part
    std::string binary = "";
    int auxReminder = 0;
    for (int i = 0; i < b - 1; i++) {
        int bit = bitStream.readBit();
        auxReminder = (auxReminder << 1) + bit;
        binary += char(bit + '0');
    }

    int extraBit = 10;
    if (auxReminder >= first_values) { 
        extraBit = bitStream.readBit();
        binary += char(extraBit + '0');
    }

    // transform binary string to int, this is the remainder
    for (char ch : binary) {
        remainder = (remainder << 1) + (ch - '0');
    }

    if (extraBit != 10) {
        int lowestValToSubstract = pow(2, b) - m;
        remainder = remainder - lowestValToSubstract;
    }

    value = quocient * m + remainder;

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