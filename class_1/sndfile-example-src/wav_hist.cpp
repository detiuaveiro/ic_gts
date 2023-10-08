#include "wav_hist.h"
#include <iostream>
#include <sndfile.hh>
#include <vector>

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536;  // Buffer for reading frames

int main(int argc, char* argv[]) {

    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <input file> <channel> <binSize>\n";
        return 1;
    }

    SndfileHandle sndFile{argv[argc - 3]};
    if (sndFile.error()) {
        cerr << "Error: invalid input file\n";
        return 1;
    }

    if ((sndFile.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
        cerr << "Error: file is not in WAV format\n";
        return 1;
    }

    if ((sndFile.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
        cerr << "Error: file is not in PCM_16 format\n";
        return 1;
    }

    size_t bFactor = stoi(argv[argc - 1]);
    if (bFactor == 0) {
        cerr << "Error: bins should gather 2^k values (k > 0)\n";
        return 1;
    }

    std::cout << "Number of channels: " << sndFile.channels() << '\n';
    int channel{stoi(argv[argc - 2])};
    if (channel == 1 && channel >= sndFile.channels() + 2) {
        cerr << "Error: invalid channel requested\n";
        return 1;
    }

    size_t nFrames;
    vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
    WAVHist hist{sndFile, bFactor};
    while ((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE))) {
        samples.resize(nFrames * sndFile.channels());
        hist.update(samples, sndFile, FRAMES_BUFFER_SIZE);
    }

    hist.dump(channel);
    return 0;
}
