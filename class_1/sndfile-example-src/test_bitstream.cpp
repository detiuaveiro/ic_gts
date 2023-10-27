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
    writer.writeBit(0);

    writer.writeBit(0);

    writer.writeBit(0);

    writer.writeNBits(3, 8);  //A7C0

    writer.writeChar('A');

    writer.writeString("Samu Tex");

    writer.writeInt(-45);

    writer.~BitStream();

    BitStream reader('r', "example.bin");

    int bit1 = reader.readBit();
    int bit2 = reader.readBit();
    int bit3 = reader.readBit();

    std::cout << "Bit1: " << bit1 << " , Bit2: " << bit2 << " , Bit3: " << bit3
              << std::endl;

    // two shift to get the previous representation (14 to 16 bits)
    long readNums = reader.readNBits(8);
    std::cout << "Read Nums: " << readNums << std::endl;

    char readCharacter = reader.readChar();
    std::cout << "Read Character: " << readCharacter << std::endl;
    std::string readString = reader.readString(64);
    std::cout << "Read String: " << readString << std::endl;
    int readInteger = reader.readInt(32);
    std::cout << "Read Int: " << readInteger << std::endl;
    std::cout << "End of File: " << reader.readInt(32) << std::endl;
    std::cout << "End of File: " << reader.check_eof() << std::endl;
    reader.~BitStream();

    return 0;
}