#include "golomb.h"

Golomb::Golomb(int m, BitStream& bitStream, APPROACH approach = SIGN_MAGNITUDE)
    : bitStream(bitStream) {
    this->m = m;
    this->approach = approach;
}

// Represent remainder as binary
int Golomb::calculate_remainder(int r) {

    // log2(m) = number of bits to represent m
    int b = ceil(log2(m));

    // m is power of 2
    if ((m & (m - 1)) == 0)
        return r;

    // m is not power of 2
    if (r < pow(2, b) - m)
        return r;  // truncate
    else
        return (r + pow(2, b) - m) % 2;
}

int Golomb::encode_sign_magnitude(int value) {
    int quotient = value / m;
    int remainder = value % m;

    bool isNegative = (value < 0) ? true : false;
    value = abs(value);

    // creating the unary with as many 1s as the quotient
    unsigned int result = (1 << quotient) - 1;

    // Insert 0 to separate the quotient from the remainder
    result <<= 1;

    result += calculate_remainder(remainder);

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
    int remainder = value % m;

    // if number is negative, multiply by 2 and subtract 1
    if (value < 0)
        value = 2 * abs(value) - 1;
    else
        value = 2 * value;

    // creating the unary with as many 1s as the quotient
    unsigned int result = (1 << quotient) - 1;

    // Insert 0 to separate the quotient from the remainder
    result <<= 1;

    result += calculate_remainder(remainder);

    return result;
}

void Golomb::encode(int value) {
    if (m <= 0)
        throw std::invalid_argument("m must be positive");

    int result = 0;
    if (approach == SIGN_MAGNITUDE)
        result = encode_sign_magnitude(value);
    else if (approach == VALUE_INTERLEAVING)
        result = encode_value_interleaving(value);
    else
        throw std::invalid_argument("Invalid approach");

    bitStream.writeNBits(result, (int)(log2(value) + 1));
}

// Decode a sequence of bits
int Golomb::decode() {

    int value = 0;
    int quotient = 0;
    int remainder = 0;

    // read the unary part until a 0 is found
    std::string unary = "";
    while (true) {
        int bit = bitStream.readBit();
        if (bit == 0)
            break;
        quotient++;
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