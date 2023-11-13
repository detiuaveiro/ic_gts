#include <golomb.h>

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
void Golomb::encode(int value) {
    std::string s = "";
    int q = 0;
    int r = 0;
    if (approach ==
        1) {  // SIGN AND MAGNITUDE -> MSB = 0 -> positive, MSB = 1 -> negative

        bool isNegative = false;
        if (value < 0) {
            isNegative = true;
            value = -value;  // Encode the positive value of i
        }

        q = floor(value / m);
        r = value % m;

        std::cout << "Unary: " << q << ", remainder: " << r << std::endl;

        for (int j = 0; j < q; j++)
            s += "1";  // representar q de forma unária
        s += "0";      // separar q de r

        // representar r de forma binária
        s += getRemainderBinary(r);

        if (value != 0) {
            if (isNegative)
                s = s + "1";
            else
                s = s + "0";
        }
        // converter s (binario) em inteiro
        long int_binary_val = 0;
        for (char c : s) {
            int_binary_val = (int_binary_val << 1) + (c - '0');
            std::cout << "writing: " << c << std::endl;
        }
        bitStream.writeNBits(int_binary_val, s.length());
    } else {  // VALUE INTERLEAVING

        // if number is negative, multiply by 2 and subtract 1
        if (value < 0)
            value = 2 * -value - 1;
        else
            value = 2 * value;

        int q = floor(value / m);  // unary part
        int r = value % m;

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

    int ISTO_ESTA_MT_MAU = 6;    // mudar
    int ISTO_ESTA_MT_MAU_2 = 6;  // mudar
    long int_binary_val = bitStream.readNBits(ISTO_ESTA_MT_MAU);
    std::string s = "";
    for (int i = ISTO_ESTA_MT_MAU_2 - 1; i >= 0; i--) {
        int bit = (int_binary_val >> i) & 1;
        s += std::to_string(bit);
    }

    // Now that i have the string s, i can decode it
    int value = 0;
    int q = 0;
    int r = 0;
    for (unsigned long i = 0; i < s.length(); i++) {
        if (s[i] == '0') {
            q = i;
            break;
        }
    }
    if (approach == 1) {  // SIGN AND MAGNITUDE

        // get the binary remainder
        for (unsigned long i = q + 1; i < s.length() - 1; i++) {
            r = (r << 1) + (s[i] - '0');
        }
        value = q * m + r;
        // last bit is the sign
        if (s[s.length() - 1] == '1')
            value = -value;
        return value;
    } else {  // VALUE INTERLEAVING

        // get the binary remainder
        for (unsigned long i = q + 1; i < s.length(); i++) {
            r = (r << 1) + (s[i] - '0');
        }
        value = q * m + r;
        // if number is odd, add 1 and divide by 2
        if (value % 2 == 1) {
            value++;
            value /= 2;
            value = -value;
        } else {
            value /= 2;
        }
        return value;
    }
}