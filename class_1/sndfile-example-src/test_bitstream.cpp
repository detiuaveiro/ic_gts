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

    /*writebit ta a pescar bem os bits*/
    writer.writeBit(0);

    writer.writeBit(0);

    writer.writeBit(0);

    writer.writeNBits(42944, 7);  //A7C0

    /*writer.writeChar('F');

    writer.writeString("Hello");*/

    writer.~BitStream();

    BitStream reader('r', "example.bin");

    int bit1 = reader.readBit();
    int bit2 = reader.readBit();
    int bit3 = reader.readBit();

    std::cout << "Bit1: " << bit1 << " , Bit2: " << bit2 << " , Bit3: " << bit3
              << std::endl;

    long readNums = reader.readNBits(7);
    std::cout << "Read Nums: " << readNums << std::endl;

    //std::string readString = reader.readString(40);
    //std::cout << "Read String: " << readString << std::endl;

    reader.~BitStream();

    return 0;
}