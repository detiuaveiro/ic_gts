#include <cmath>
#include <fstream>
#include <ios>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include "bit_stream.h"

int main() {
    BitStream writer('w', "example.bin");

    //writebit ta a pescar bem os bits
    writer.writeBit(1);

    writer.writeBit(1);

    writer.writeBit(1);

    writer.writeNBits(42944, 14);  //A7C0

    writer.writeChar('Z');

    writer.writeString("Samu T");

    writer.writeInt(-1);

    writer.~BitStream();

    BitStream reader('r', "example.bin");

    int bit1 = reader.readBit();
    int bit2 = reader.readBit();
    int bit3 = reader.readBit();

    std::cout << "Bit1: " << bit1 << " , Bit2: " << bit2 << " , Bit3: " << bit3
              << std::endl;

    long readNums = reader.readNBits(14);
    std::cout << "Read Nums: " << readNums << std::endl;

    char readCharacter = reader.readChar();
    std::cout << "Read Character: " << readCharacter << std::endl;
    std::string readString = reader.readString(48);
    std::cout << "Read String: " << readString << std::endl;
    int readInteger = reader.readInt(32);
    std::cout << "Read Int: " << readInteger << std::endl;

    reader.~BitStream();

    return 0;
}