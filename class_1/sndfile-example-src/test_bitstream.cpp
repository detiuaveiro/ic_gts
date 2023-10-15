#include "bit_stream.h"
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <ios>
#include <cmath>

int main()
{
    BitStream writer('w', "example.bin");

    writer.writeNBits('3');

    writer.writeNBits('A');

    writer.writeString("Hello");

    writer.~BitStream();

    BitStream reader('r', "example.bin");

    char readNumber = reader.readNBits(2);
    std::cout << "Read number: " << readNumber << std::endl;

    char readChar = reader.readNBits(8);
    std::cout << "Read Character: " << readNumber << std::endl;

    std::string readString = reader.readString(40);
    std::cout << "Read String: " << readString << std::endl;

    reader.~BitStream();

    return 0;
}