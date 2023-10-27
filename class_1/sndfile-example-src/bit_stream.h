#include <bits/stdc++.h>
#include <cmath>
#include <fstream>
#include <ios>
#include <iostream>
#include <map>
#include <string>
#include <vector>

class BitStream {
   private:
    std::fstream file;
    char mode;
    char buffer;
    short buffer_index;

   public:
    BitStream(char mode, std::string fileName) {
        this->mode = mode;
        if (mode == 'r' || mode == 'R') {
            this->file.open(fileName, std::fstream::in | std::fstream::binary);
            this->buffer_index = -1;
        } else if (
            mode == 'w' ||
            mode ==
                'W') {  // https://cplusplus.com/reference/fstream/fstream/fstream/ second arg is a bitmask
            this->file.open(fileName, std::fstream::out | std::fstream::binary);
            this->buffer_index = 7;
            this->buffer = 0;
        } else {
            std::cerr << "Error: mode " << mode << " unrecognizable";
        }
    }

    // Writes a bit into the file
    void writeBit(int bit) {
        buffer |= (bit & 1) << buffer_index;
        buffer_index--;

        //std::cout << "Bit write: " << bit << std::endl;
        if (buffer_index < 0) {
            file.put(buffer);
            buffer = 0;
            buffer_index = 7;
        }
    }

    // Reads a bit from the file
    int readBit() {
        if (buffer_index < 0) {
            file.get(buffer);
            buffer_index = 7;
        }

        int bit = (buffer >> buffer_index) & 1;

        //std::cout << "Bit read: " << bit << std::endl;

        buffer_index--;
        return bit;
    }

    // Writes N bits to file
    void writeNBits(long value, int n_bits) {
        if (value == 0) {
            writeBit(0);
            return;
        }

        short bits_to_represent_value = log2(value) + 1;

        //If the requested bits to represent value is bigger than the minimum needed
        if (bits_to_represent_value < n_bits) {
            bits_to_represent_value = n_bits;
        }
        // Write the remaining bits from the value
        //std::cout << bits_to_represent_value - n_bits << std::endl;

        for (int i = bits_to_represent_value - 1;
             i >= bits_to_represent_value - n_bits; i--) {
            int x = (value >> i) & 1;
            writeBit(x);
        }
    }

    //Writes an integer to file
    void writeInt(int integer) {
        if (integer == 0)
            writeBit(0);
        short bits_to_represent_integer = log2(integer) + 1;
        if (integer < 0) {
            bits_to_represent_integer = 32;
        }
        for (int i = bits_to_represent_integer - 1; i >= 0; i--) {
            writeBit((integer >> i) & 1);
        }
    }

    //Writes an ascii character to file
    void writeChar(char character) {
        for (int i = 7; i >= 0; i--) {
            writeBit((character >> i) & 1);
        }
    }

    // Reads N bits from file
    long readNBits(int size) {
        long byte = 0;
        for (int i = size - 1; i >= 0; i--) {
            byte |= readBit() << i;
        }

        return byte;
    }

    //Reads an integer from file
    int readInt(int size) {
        int byte = 0;
        for (int i = size - 1; i >= 0; i--) {
            byte |= readBit() << i;
        }

        return byte;
    }

    //Reads an ascii character from file
    char readChar() {
        char byte = 0;
        for (int i = 7; i >= 0; i--) {
            byte |= readBit() << i;
        }

        return byte;
    }

    // Writes string into file
    void writeString(std::string str) {
        for (char chr : str) {
            writeChar(chr);
        }
    }

    // Reads string from file
    std::string readString(int size) {
        std::string string;

        for (int i = 0; i < size / 8; i++) {
            string += readChar();
        }

        return string;
    }

    bool check_eof() { return file.eof(); }

    ~BitStream() {
        if ((this->mode == 'w' || this->mode == 'W') && buffer_index < 7) {
            //std::cout << "Entered deconstructor " << std::endl;

            this->file.put(buffer);
            buffer = 0;
        }
    }

    // Get file size in Bytes
    int fileSizeBytes() {
        std::streampos currentPosition = file.tellg();
        file.seekg(0, std::ios::end);
        int sizeBytes = static_cast<int>(file.tellg());
        file.seekg(currentPosition);

        return sizeBytes;
    }
};
