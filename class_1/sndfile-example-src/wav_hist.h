#ifndef WAVHIST_H
#define WAVHIST_H

#include <iostream>
#include <vector>
#include <map>
#include <sndfile.hh>

class WAVHist {
  private:
	// counts[value][counter]
	std::vector<std::map<short, size_t>> counts;
	size_t nChannels;

  public:
	WAVHist(const SndfileHandle& sfh) {
		if(sfh.channels() > 2)
			throw std::invalid_argument("WAVHist can only handle 1 or 2 channels");

		// Accounting for Mid and Side channels
		counts.resize(sfh.channels() + 2);
		nChannels = sfh.channels();
	}

	// The vector is in format: LR LR LR...
	void update(const std::vector<short>& samples) {
		size_t n { };
		for(size_t i = 0; i < samples.size(); i++){
			counts[n++ % nChannels][samples[i]]++;
			if(i > 0 && i%2 != 0){
				int left = i - 1;
				int right = i;
				counts[2][samples[right]]=(left-right)/2; // Side
				counts[3][samples[left]]=(left+right)/2; // Mid
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

