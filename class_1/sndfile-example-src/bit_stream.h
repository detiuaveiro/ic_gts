#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <ios>
#include <cmath>

class BitStream
{
private:
    std::fstream file;
    char mode;
    char buffer;
    short buffer_index;

public:
    BitStream(char mode, std::string fileName)
    {
        if (mode == 'r' || mode == 'R')
        {
            this->file.open(fileName, std::fstream::in | std::fstream::binary);
            this->buffer_index = -1;
        }
        else if (mode == 'w' || mode == 'W')
        { // https://cplusplus.com/reference/fstream/fstream/fstream/ second arg is a bitmask
            this->file.open(fileName, std::fstream::out | std::fstream::binary);
            this->buffer_index = 7;
        }
        else
        {
            std::cerr << "Error: mode " << mode << " unrecognizable";
        }
    }

    // Writes a bit into the file
    void writeBit(int bit)
    {
        buffer |= (bit & 1) << buffer_index;
        buffer_index--;

        if (buffer_index < 0)
        {
            file.put(buffer);
            buffer = 0;
            buffer_index = 7;
        }
    }

    // Reads a bit from the file
    int readBit()
    {
        if (buffer_index < 0)
        {
            file.get(buffer);
            buffer_index = 7;
        }

        int bit = (buffer >> buffer_index) & 1;
        buffer_index--;

        return bit;
    }

    // Converts a byte into the corresponding bitArray
    std::vector<int> byteToBitArray(char byte)
    {
        std::vector<int> bitArray;

        for (size_t i = 0; i <= 7; i--)
        {
            bitArray.push_back((byte >> i) & 1);
        }
        return bitArray;
    }

    // Converts a bitArray into the corresponding byte
    char bitArrayToByte(std::vector<int> bitArray)
    {
        char byte = 0;
        for (size_t i = 0; i < 8; i++)
        {
            byte |= bitArray[i] << i;
        }

        return byte;
    }

    // Writes N bits to file
    void writeNBits(char byte)
    {
        std::vector<int> bitArray = byteToBitArray(byte);

        if (isdigit(byte))
        {
            // https://www.geeksforgeeks.org/count-total-bits-number/ number of bits to represent a number
            int bits = (int)log2((double)byte) + 1;
            for (size_t i = 0; i < bits; i++)
            {
                writeBit(bitArray[i]);
            }
        }
        else
        {
            for (int bit : bitArray)
            {
                writeBit(bit);
            }
        }
    }

    // Reads N bits from file
    char readNBits(int size)
    {
        std::vector<int> bitArray;
        for (size_t i = 0; i < size; i++)
        {
            bitArray.push_back(readBit());
        }

        return bitArrayToByte(bitArray);
    }

    // Writes string into file
    void writeString(std::string str)
    {
        std::vector<char> chars;
        for (char chr : str)
        {
            writeNBits(chr);
        }
    }

    // Reads string from file
    std::string readString(int size)
    {
        std::string string;

        for (size_t i = 0; i < size / 8; i++)
        {
            string += readNBits(8);
        }

        return string;
    }
};
