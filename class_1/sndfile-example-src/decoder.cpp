#include <iostream>
#include <sndfile.hh>
#include "bit_stream.h"

using namespace std;
// CONVERT BINARY FILE INTO TEXT FILE 0s and 1s EQUIVALENT

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		cerr << "Usage: " << argv[0] << " <input file> <output file>\n";
		return 1;
	}

    string inFile = argv[1];
    BitStream inputF{'r', inFile};

    std::ofstream outFile(argv[2],ios::out);

    if (!outFile.is_open()) {
        cerr << "Error: Unable to open the output file: " << argv[1] << endl;
        return 1;
    }

    // Read N bits from binary file (obtain long value)
    // *Need to get file size to get the entire file size*
    // The file size will be multiplied by 8 because each byte on the binary file represents 8 bits on the text file
    int fileSize = inputF.fileSizeBytes();
    long longValue = inputF.readNBits(fileSize*8);

    // Transform long value to string containing 0s and 1s
    std::string binaryValue = "";
    if (longValue == 0){
        binaryValue = "0";
    }
    while (longValue > 0){
        binaryValue = std::to_string(longValue % 2) + binaryValue;
        longValue /= 2;
    }

    // Write 0s and 1s to the output file
    outFile << binaryValue; 

    outFile.close();

    /* Run test:
        ../sndfile-example-bin/decoder encoded.bin decoded.txt 
    */

    return 0;
}