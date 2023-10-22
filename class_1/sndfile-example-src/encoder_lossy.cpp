#include <fftw3.h>
#include <cmath>
#include <iostream>
#include <sndfile.hh>
#include <string>
#include <vector>
#include "bit_stream.h"
#include "wav_effects.h"

using namespace std;

// configurable parameters
namespace Options {
string musicName = "sample.wav";
string encodedName = "encodedMusic";
}  // namespace Options

static void print_usage() {
    cerr
        << "Usage: %s [OPTIONS]\n"
           "  OPTIONS:\n"
           "  -h, --help        --- print this help\n"
           "  -i, --input       --- set music file name (default: sample.wav)\n"
           "  -o, --output      --- set encoded file name (default: "
           "encodedMusic)\n"
        << endl;
}

int check_wav_file(SndfileHandle& musicFile) {
    if (musicFile.error()) {
        cerr << "Error: invalid input file\n";
        return -1;
    }

    if ((musicFile.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
        cerr << "Error: file is not in WAV format\n";
        return -1;
    }

    if ((musicFile.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
        cerr << "Error: file is not in PCM_16 format\n";
        return -1;
    }

    return 0;
}

int process_arguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {  // Start for at 1, to skip program name
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage();
            return 1;
        } else if (strcmp(argv[i], "-i") == 0 ||
                   strcmp(argv[i], "--input") == 0) {
            i++;
            if (i < (argc - 1)) {
                Options::musicName = argv[i];
            } else {
                std::cerr << "Error: Missing argument for -i/--input option."
                          << std::endl;
                return -1;
            }

        } else if (strcmp(argv[i], "-o") == 0 ||
                   strcmp(argv[i], "--output") == 0) {
            i++;
            if (i < (argc - 1)) {
                Options::musicName = argv[i];
            } else {
                std::cerr << "Error: Missing argument for -o/--output option."
                          << std::endl;
                return -1;
            }

        } else if (argv[i][0] == '-') {
            std::cerr << "Error: Unknown option or argument: " << argv[i]
                      << std::endl;
            return -1;
        }
    }
    return 0;
}

void transform_music_mono(SndfileHandle& sfhIn, SndfileHandle& sfhOut) {

    std::vector<short> inputSamples(FRAMES_BUFFER_SIZE * sfhIn.channels());
    std::vector<short> outputSamples;
    vector<double> arg;
    size_t nFrames;

    WAVEffects effects{MONO, arg, sfhIn.samplerate()};

    while ((nFrames = sfhIn.readf(inputSamples.data(), FRAMES_BUFFER_SIZE))) {
        inputSamples.resize(nFrames * sfhIn.channels());

        effects.effect_mono(inputSamples, outputSamples);
    }

    sfhOut.writef(outputSamples.data(), outputSamples.size());
}

int main(int argc, char* argv[]) {
    int ret = process_arguments(argc, argv);
    if (ret < 0)
        return 1;
    else if (ret > 0)
        return 0;

    /* Check .wav files */
    SndfileHandle sfhTmp{Options::musicName};
    if (check_wav_file(sfhTmp) < 0)
        return 1;

    int channels = sfhTmp.channels();
    // Check if the music is mono channel and ask the user if we want's to transform it
    if (channels > 1) {
        cout << "The music isn't mono channel, do you wish to transform it? "
                "(y/n): ";
        string input;
        cin >> input;

        if (input != "y" && input != "Y")
            return 0;

        string newMusicName = Options::musicName;

        size_t dotPos = newMusicName.find_last_of('.');

        if (dotPos != std::string::npos) {
            // Remove the extension (including .)
            newMusicName = newMusicName.substr(0, dotPos);
        }

        newMusicName = newMusicName + "_mono.wav";
        SndfileHandle sfhOut{newMusicName, SFM_WRITE, sfhTmp.format(), 1,
                             sfhTmp.samplerate()};

        if (sfhOut.error()) {
            cerr << "Error: problem transforming stereo music into mono"
                 << newMusicName << endl;
            return 1;
        }

        transform_music_mono(sfhTmp, sfhOut);
        Options::musicName = newMusicName;

        cout << " - Music transformed into mono and saved in " << newMusicName
             << endl;
    }

    /* Check .wav files */
    SndfileHandle sfhIn{Options::musicName};
    if (check_wav_file(sfhIn) < 0)
        return 1;

    return 0;
}