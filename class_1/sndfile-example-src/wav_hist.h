#ifndef WAVHIST_H
#define WAVHIST_H

#include <iostream>
#include <vector>
#include <map>
#include <sndfile.hh>

class WAVHist {
  private:
	// counts[value][counter]
	std::vector<std::map<short, unsigned int>> counts;
	size_t nChannels;
	size_t binningFactor;

  public:
	WAVHist(const SndfileHandle& sfh, size_t bFactor = 1) {
		if(sfh.channels() > 2)
			throw std::invalid_argument("WAVHist can only handle 1 or 2 channels");

		// Accounting for Mid and Side channels
		counts.resize(sfh.channels() + 2);
		nChannels = sfh.channels();
		
		if(bFactor % 2 != 0)
			throw std::invalid_argument("Binning Factor must be even");

		binningFactor = bFactor;
	}

	// The vector is in format: LR LR LR...
	void update(const std::vector<short>& samples) {
		/*
			Constant value of bits to shift by: is (-2^15 + 2^15)/binningFactor ???? 
				or max_min + max_max representation
		*/
		size_t n { };
		for(int i = 0; i < samples.size(); i++){
			counts[n++ % nChannels][samples[i]]++;
			if(i > 0 && i%2 != 0){
				short left = samples[i - 1];
				short right = samples[i];
				counts[2][(left-right)/2]++; // Side
				counts[3][(left+right)/2]++; // Mid
			}
		}	
	}

	void dump(const size_t channel) const {
		std::cout << "Value" << '\t' << "Count" <<  '\n';
		for(auto [value, counter] : counts[channel])
			std::cout << value << '\t' << counter << '\n';
	}
};

#endif

