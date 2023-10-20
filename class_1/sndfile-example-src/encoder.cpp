#include <iostream>
#include <sndfile.hh>
#include "bit_stream.h"

using namespace std;
// CONVERT TEXT FILE WITH 0s and 1s INTO BINARY EQUIVALENT
// (each byte in the binary file represents 8 bits in the text file)
int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		cerr << "Usage: " << argv[0] << " <input file> <output file>\n";
		return 1;
	}

    std::ifstream inFile(argv[1], ios::in);
    if (!inFile.is_open()) {
        cerr << "Error: Unable to open the input file: " << argv[1] << endl;
        return 1;
    }

    std::string outFile = argv[2];

    // Get bits from the input file
    std::string fileBits;
    getline(inFile, fileBits);

    // Ensure that the fileBits contain only '0's and '1's
    for (char c : fileBits) {
        if (c != '0' && c != '1') {
            std::cerr << "Error: Input file should contain only '0's and '1's.\n";
            inFile.close();
            return 1;
        }
    }

    // Convert bits to a long value
    long longValue = 0;
    for (char c : fileBits) {
        longValue = (longValue << 1) + (c - '0');
    }

    // Write to the binary file
    BitStream outF{'w',outFile};
    outF.writeNBits(longValue,fileBits.length());
    outF.~BitStream();
    

    /* Run test:
        ../sndfile-example-bin/encoder zeros_ones.txt encoded.bin    
    */

    return 0;
}