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


    int fileSize = inputF.fileSizeBytes();
    for (int i = 0; i < fileSize; i++) {
        // **Read and conversion to binary string seems alright for now**
        long longValue = inputF.readNBits(8); 
        cout << "Long Value: " << longValue << "\n";
        std::string binaryValue = "";
        if (longValue == 0){
            binaryValue = "0";
        }
        while (longValue > 0){
            binaryValue = std::to_string(longValue % 2) + binaryValue;
            longValue /= 2;
        }
        outFile << binaryValue;
    }

    outFile.close();

    /* Run test:
        ../sndfile-example-bin/decoder encoded.bin decoded.txt 
    */

    return 0;
}