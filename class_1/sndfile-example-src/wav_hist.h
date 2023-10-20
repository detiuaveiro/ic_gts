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

    // The vector is in format: LR LR LR...
    void update(const std::vector<short>& samples) {
        /*
			Constant value of bits to shift by: is (-2^15 - 2^15)/binningFactor ????
				or max_min + max_max representation
            
            numBins = l = (INT16_MAX - INT16_MIN) / binningFactor
		*/

        // take into account that when 1 is introduced, no shifts should be done
        short binSize = binningFactor - 1;

        size_t n{};
        for (size_t i = 0; i < samples.size(); i++) {
            short index = samples[i] >> binSize;
            counts[n++ % nChannels][index]++;
            if (i > 0 && i % 2 != 0) {
                short left = samples[i - 1] >> binSize;
                short right = samples[i] >> binSize;
                counts[2][(left - right) / 2]++;  // Side
                counts[3][(left + right) / 2]++;  // Mid
            }
        }
    }

    void dump(const size_t channel) const {
        for (size_t i = 0; i < channel; i++) {
            switch (i) {
                case 0:
                    std::cout << "Value" << '\t' << "Left Count" << '\n';
                    break;
                case 1:
                    std::cout << "Value" << '\t' << "Right Count" << '\n';
                    break;
                case 2:
                    std::cout << "Value" << '\t' << "Mid Count" << '\n';
                    break;
                case 3:
                    std::cout << "Value" << '\t' << "Side Count" << '\n';
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
