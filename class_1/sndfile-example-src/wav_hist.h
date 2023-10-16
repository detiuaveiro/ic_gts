#ifndef WAVHIST_H
#define WAVHIST_H

#include <cmath>
#include <iostream>
#include <map>
#include <sndfile.hh>
#include <vector>

using namespace std;

class WAVHist {
   private:
    // counts[value][counter]
    std::vector<std::map<short, unsigned int>> counts;
    size_t nChannels;
    size_t binningFactor;

   public:
    WAVHist(const SndfileHandle& sfh, size_t bFactor = 1) {
        if (sfh.channels() > 2)
            throw std::invalid_argument(
                "WAVHist can only handle 1 or 2 channels");

        // Accounting for Mid and Side channels
        counts.resize(sfh.channels() + 2);
        nChannels = sfh.channels();

        binningFactor = bFactor;
    }

    std::vector<short> createBin(size_t binningFactor,
                                 const std::vector<short>& samples) {

        long numBins = (INT16_MAX - INT16_MIN) / binningFactor;  //l

        std::vector<short> binnedSamples;
        binnedSamples.resize(numBins);

        for (long i = 0; i < (long)samples.size(); i += binningFactor) {
            int lVal = 0, rVal = 0;
            for (long j = i; j < (i + binningFactor); j += 2) {
                lVal += samples[j];
                rVal += samples[j + 1];
            }
            int sValue = int((lVal + rVal) / 2);
            int index = int(sValue / numBins);  // x/l
            binnedSamples[index] = sValue;
        }

        return binnedSamples;

        /*std::vector<short> binnedSamples;

        short Lsum = 0, Rsum = 0;
        int binnedSamplesCounter = 0;
        for (size_t i = 0; i < samples.size(); i++) {
            if (i % 2 == 0 || i == 0) {
                Lsum += samples[i];
            } else {
                Rsum += samples[i];
            }
            int power = pow(2, binningFactor);
            // When index reaches the last of the bin
            // i.e. bFactor = 2 (2 lefts and 2 rights) ==> last index = 3, 7, 11, 15, ...
            if (i % (power - 1) == 0) {
                binnedSamples[binnedSamplesCounter] = Lsum / (power / 2);
                binnedSamples[binnedSamplesCounter + 1] = Rsum / (power / 2);
                Lsum = Rsum = 0;
                binnedSamplesCounter += 2;
            }
        }
        return binnedSamples;*/
    }
    // The vector is in format: LR LR LR...
    void update(const std::vector<short>& samples) {
        /*
			Constant value of bits to shift by: is (-2^15 - 2^15)/binningFactor ????
				or max_min + max_max representation
		*/
        if (binningFactor > 0) {
            std::vector<short> newSamples = createBin(binningFactor, samples);
            createMidSide(newSamples);
        } else {
            createMidSide(samples);
        }
    }

    void createMidSide(const std::vector<short>& samples) {
        size_t n{};
        for (size_t i = 0; i < samples.size(); i++) {
            counts[n++ % nChannels][samples[i]]++;
            if (i > 0 && i % 2 != 0) {
                short left = samples[i - 1];
                short right = samples[i];
                counts[2][(left - right) / 2]++;  // Side
                counts[3][(left + right) / 2]++;  // Mid
            }
        }
    }

    void dump(const size_t channel) const {
        for (size_t i = 0; i < channel; i++) {
            switch (i) {
                case 0:
                    std::cout << "Value" << '\t' << "Count" << '\t' << "Left"
                              << '\n';
                    break;
                case 1:
                    std::cout << "Value" << '\t' << "Count" << '\t' << "Right"
                              << '\n';
                    break;
                case 2:
                    std::cout << "Value" << '\t' << "Count" << '\t' << "MID"
                              << '\n';
                    break;
                case 3:
                    std::cout << "Value" << '\t' << "Count" << '\t' << "SIDE"
                              << '\n';
                    break;
                default:
                    break;
            }
            for (auto [value, counter] : counts[i])
                std::cout << value << '\t' << counter << '\n';
        }
    }
};

#endif
