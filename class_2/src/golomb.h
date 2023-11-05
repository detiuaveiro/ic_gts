#include <iostream>
#include <string>
#include <cmath>

// Functions for:
// - Encoding an integer (generating the corresponding sequence of bits)
// - Decoding a sequence of bits (generating the corresponding integer)

// Parameter m to allow fitting the codes to the appropriate probability distribution
// m is the number of bits used to represent the remainder

class Golomb {
    private:
        int m;

        // Representar r de forma binária
        std::string getRemainderBinary(int r, int num)
        {
            std::string s = "";
            
            if ((m & (m - 1)) == 0) // m é potência de 2
            {
                for (int i = 0; i < log2(m); i++){
                    s += std::to_string(r % 2);
                    r /= 2;
                }
            }
            else // truncar o código binário (m não é potência de 2)
            {
                int b = ceil(log2(m));
                // encode primeiros 2^b - m valores de r usando b - 1 bits
                if (r < pow(2, b) - m){
                    for (int i = 0; i < b - 1; i++){
                        s += std::to_string(r % 2);
                        r /= 2;
                    }
                }else{ // encode os restantes valores de r usando b bits
                    r += pow(2, b) - m;
                    for (int i = 0; i < b; i++){
                        s += std::to_string(r % 2);
                        r /= 2;
                    }
                }
            }

            return s;
        }

    public:
        Golomb(int m){
            this->m = m;
        }

        // Inteiro i é representado por 2 números: q e r
        // q é a parte inteira da divisão de n por m (parte unária)
        // r é o resto da divisão de n por m (parte binária)
        std::string encode(int i)
        {
            std::string s = "";
            int q = i / m;
            int r = i % m;

            for (int j = 0; j < q; j++)
                s += "1"; // representar q de forma unária
            s += "0"; // separar q de r

            // representar r de forma binária
            s += getRemainderBinary(r, i);

            return s;
        }

        // Decode a sequence of bits
        int decode(std::string s){
            return 0;
        }

        
};