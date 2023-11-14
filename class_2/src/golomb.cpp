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

// Representar r de forma binária
std::string Golomb::getRemainderBinary(int r) {
    std::string s = "";

    if ((m & (m - 1)) == 0)  // m é potência de 2
    {
        for (int i = 0; i < ceil(log2(m));
             i++) {  // log2(m) = nº bits para representar r
            s = std::to_string(r % 2) + s;
            r /= 2;
        }
        //remainder_bits.push_back(ceil(log2(m)));
    } else  // truncar o código binário (m não é potência de 2)
    {
        int b = ceil(log2(m));
        // encode primeiros 2^b - m valores de r usando b - 1 bits
        if (r < pow(2, b) - m) {
            for (int i = 0; i < b - 1; i++) {
                s = std::to_string(r % 2) + s;
                r /= 2;
            }
            //remainder_bits.push_back(b - 1);
        } else {  // encode os restantes valores de r usando b bits
            r += pow(2, b) - m;
            for (int i = 0; i < b; i++) {
                s = std::to_string(r % 2) + s;
                r /= 2;
            }
            //remainder_bits.push_back(b);
        }
    }

    return s;
}

// Inteiro i é representado por 2 números: q e r
// q é a parte inteira da divisão de n por m (parte unária)
// r é o resto da divisão de n por m (parte binária)
void Golomb::encode(int i) {
    std::string s = "";
    int q = 0;
    int r = 0;
    if (approach == 1) {
        // SIGN AND MAGNITUDE -> MSB = 0 -> positive, MSB = 1 -> negative

        bool isNegative = false;
        if (i < 0) {
            isNegative = true;
            i = -i;  // Encode the positive value of i
        }

        q = floor(i / m);
        r = i % m;

        for (int j = 0; j < q; j++)
            s += "1";  // representar q de forma unária
        s += "0";      // separar q de r

        // representar r de forma binária
        s += getRemainderBinary(r);

        if (i != 0) {
            if (isNegative)
                s = s + "1";
            else
                s = s + "0";
        }
        // converter s (binario) em inteiro
        long int_binary_val = 0;
        for (char c : s) {
            int_binary_val = (int_binary_val << 1) + (c - '0');
        }
        bitStream.writeNBits(int_binary_val, s.length());

    } else {
        // VALUE INTERLEAVING

        // if number is negative, multiply by 2 and subtract 1
        if (i < 0) {
            i = -i;
            i = 2 * i - 1;
        } else {
            i = 2 * i;
        }
        int q = floor(i / m);
        int r = i % m;

        for (int j = 0; j < q; j++)
            s += "1";  // representar q de forma unária
        s += "0";      // separar q de r

        // representar r de forma binária
        s += getRemainderBinary(r);

        // converter s (binario) em inteiro
        long int_binary_val = 0;
        for (char c : s) {
            int_binary_val = (int_binary_val << 1) + (c - '0');
        }
        bitStream.writeNBits(int_binary_val, s.length());
    }
}

// Decode a sequence of bits
int Golomb::decode() {

    int value = 0;
    int q = 0;
    int r = 0;

    // read the unary part until a 0 is found
    std::string unary = "";
    while(true){
        int bit = bitStream.readBit();
        if(bit == 0)
            break;
        std::cout << "bit Reading Quoficient: " << bit << std::endl;
        q++;
    }
    std::cout << "q obtained: " << q << std::endl;

    int b = ceil(log2(m));
    int first_values = pow(2, b) - m;

    // read the binary part
    std::string binary = "";
    int auxReminder = 0;
    int bit = 0;
    for(int i = 0; i < b - 1; i++){
        int bit = bitStream.readBit();
        std::cout << "bit Reading Remainder: " << bit << std::endl;
        //binary_part += char(bit + '0');
        auxReminder = (auxReminder << 1) + bit;
        binary += char(bit + '0');
    }
    std::cout << "auxReminder: " << auxReminder << std::endl;

    int extraBit = 0;
    if (auxReminder >= first_values) {
        extraBit = bitStream.readBit();
        
        //binary += char(bit + '0');
    }
    std::cout << "BINARY: " << binary << std::endl;
    // transform binary string to int, this is the remainder
    for (char ch : binary) {
        r = (r << 1) + (ch - '0'); 
    }
    // DOESNT WORK FOR HIGHER M's
    if (extraBit == 1){
        r++;
    }

    std::cout << "bit Reading Remainder(extra bit): " << extraBit << std::endl;
    std::cout << "r obtained: " << r << std::endl;

    value = q * m + r;

    if (value == 0)
        return value;
    if (approach == 1) { 
        // SIGN AND MAGNITUDE
        
        int sign = bitStream.readBit(); // Last bit is the sign
        std::cout << "sign: " << sign << std::endl;
        if (sign == 1)
            value = -value;
        return value;

    } else {
        // VALUE INTERLEAVING

        if (value % 2 == 1) { // if number is odd, add 1 and divide by 2
            value++;
            value /= 2;
            value = -value;
        } else {
            value /= 2;
        }
        return value;
    }
}