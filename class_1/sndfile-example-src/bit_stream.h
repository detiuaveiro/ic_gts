#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <ios>

class BitStream
{
private:
    std::fstream file;
    char mode;
    std::vector<char> buffer;
    short buffer_index;

public:
    BitStream(char mode, std::string fileName)
    {
        if (mode == 'r')
            this->file.open(fileName, std::fstream::in | std::fstream::binary);
        else
            // https://cplusplus.com/reference/fstream/fstream/fstream/ second arg is a bitmask
            this->file.open(fileName, std::fstream::out | std::fstream::binary);
        this->buffer_index = 7;
    }

    void writeBit(short bit)
    {
        this->buffer[buffer_index] |= bit;
        if (buffer_index == 0)
        {
            for (size_t i = 7; i >= 0; i--)
            {
                this->file << buffer[i];
            }
            buffer.clear();
            buffer_index = 7;
        }
        else
            buffer_index--;
    }

    // a bit can't be read directly from file, only bytes
    char readBit(char byte)
    {
        this->buffer[buffer_index] = byte >> buffer_index;
        if (buffer_index == 0)
            buffer_index = 7;
        else
            buffer_index--;
    }

    void writeNBits(std::vector<char> bits)
    {
        short N = bits.size();
        for (short i = N - 1; i >= 0; i--)
        {
            //if bits left to read are smaller than a byte, don't wait
            //for buffer to be entirely full
            if (i > 8)
            {
                while (this->buffer.size() != 8);
            }
            writeBit(bits[i]);
        }
    }

    std::vector<char> readNBits(int N)
    {
        std::vector<char> readBits;
        int counter;
        while (1)
        {
            // check if a full byte can be read or not
            counter = N - 8;
            if (counter >= 0)
            {
                counter = 8;
                N -= 8;
            }
            else
                counter = N;
            char byte = this->file.get();
            for (int i = 0; i < counter; i++)
            {
                while (this->buffer.size() == 8)
                    ;
                readBit(byte);
            }
            //if the last bits were read get out of the loop
            if (counter == N) break;
        }

        return readBits;
    }
};