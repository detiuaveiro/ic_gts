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

        std::cout << buffer << std::endl;

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
        buffer_index--;

        return bit;
    }

    // Converts a byte into the corresponding bitArray
    std::vector<int> byteToBitArray(char byte, short bitSize) {
        std::vector<int> bitArray;

        for (size_t i = 0; i <= bitSize; i--) {
            bitArray.push_back((byte >> i) & 1);
        }
        return bitArray;
    }

    // Converts a bitArray into the corresponding byte
    long bitArrayToBytes(std::vector<int> bitArray, short size) {
        long byte = 0;
        for (size_t i = 0; i < size; i++) {
            byte |= bitArray[i] << i;
        }

        return byte;
    }

    // Writes N bits to file
    void writeNBits(long value, int n_bits) {
        //shift right erases the LSB's, not the MSB's
        std::cout << n_bits << std::endl;
        short bits_to_represent_value = log2(value) + 1;
        for (int i = bits_to_represent_value - 1;
             i >= bits_to_represent_value - n_bits; i--) {
            int x = (value >> i) & 1;
            std::cout << "x: " << x << std::endl;
            writeBit(x);
        }
    }

    //Writes an integer to file
    void writeInt(int integer) {
        for (size_t i = 0; i < 32; i++) {
            writeBit((integer >> i) & 1);
        }
    }

    //Writes an ascii character to file
    void writeChar(char character) {
        for (size_t i = 0; i < 8; i++) {
            writeBit((character >> i) & 1);
        }
    }

    // Reads N bits from file
    long readNBits(int size) {
        std::vector<int> bitArray;

        long byte = 0;
        for (int i = size - 1; i >= 0; i--) {
            byte |= readBit() << i;
        }

        return byte;
    }

    // Writes string into file
    void writeString(std::string str) {
        std::vector<char> chars;
        for (char chr : str) {
            writeNBits(chr, 8);
        }
    }

    // Reads string from file
    std::string readString(int size) {
        std::string string;

        for (size_t i = 0; i < size / 8; i++) {
            string += readNBits(8);
        }

        return string;
    }

    ~BitStream() {
        std::cout << "Entered deconstructor" << std::endl;
        if ((this->mode == 'w' || this->mode == 'W') && buffer_index < 7) {
            this->file.put(buffer);
            std::cout << "Entered condition" << std::endl;

            buffer = 0;
        }
    }
};
