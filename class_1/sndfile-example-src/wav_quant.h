#ifndef WAVQUANT_H
#define WAVQUANT_H

#include <iostream>
#include <vector>
#include <map>
#include <sndfile.hh>

using namespace std;

class WAVQuant
{
private:
    vector<short> quantSamples;

public:
    WAVQuant(){
        quantSamples.resize(0);
    }

    void quantize(const vector<short> &samples, int quantBits)
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
};

#endif
