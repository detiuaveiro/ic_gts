#ifndef WAVQUANT_H
#define WAVQUANT_H

#include <iostream>
#include <vector>
#include <map>
#include <sndfile.hh>
#include <fstream>

using namespace std;

class WAVQuant
{
private:
    vector<short> quantSamples;
    int quantBits;

public:
    WAVQuant(int quantBits){
        quantSamples.resize(0);
        this->quantBits = quantBits;
    }

    void quantize(const vector<short> &samples)
    {
        int cutBits = 16 - quantBits; 
        for (auto sample : samples){
            sample >>= cutBits; // bits menos significativos a 0
            short tmp = sample << cutBits; // posicao original
            quantSamples.insert(quantSamples.end(),tmp);
        }
    }

    void toFile(SndfileHandle sfhOut) {
        sfhOut.write(quantSamples.data(), quantSamples.size());
    }

    void saveQuantizedSamples(const string &filename) {
        ofstream outputFile(filename);
        if (!outputFile.is_open()) {
            cerr << "Error: Couldn't open the output file for writing." << endl;
            return;
        }

        for (auto sample : quantSamples) {
            outputFile << sample << endl;
        }

        outputFile.close();
    }

};

#endif
