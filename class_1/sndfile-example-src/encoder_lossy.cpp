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
size_t blockSize = 1024;
size_t quantizationLevels = 8;  // or 256 levels
double dctFrac = 0.2;
}  // namespace Options

static void print_usage() {
    cerr
        << "Usage: %s [OPTIONS]\n"
           "  OPTIONS:\n"
           "  -h, --help        --- print this help\n"
           "  -i, --input       --- set music file name (default: sample.wav)\n"
           "  -o, --output      --- set encoded file name (default: "
           "encodedMusic)\n"
           "  -b, --blockSize   --- set block size (default: 1024)\n"
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

/* 
    Because of this "The decoder should rely only on the (binary) file 
        produced by the encoder in order to reconstruct (an approximate 
        version of) the audio", we need to include a sort of header in the 
        file with all the parameters 
*/
void create_file_header(SndfileHandle& sfhIn) {
    // total number of blocks
    // sample rate
    // total frames number
    // No need to store number of channels, since mono. right...?
    // ?
    
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

// Function to apply DCT to an audio block
vector<vector<double>> apply_dct(const std::vector<short>& audioBlock) {
    // dummy code just to skip warnings
    cout << audioBlock.size() << endl;
    vector<vector<double>> result;
    return result;
}

// Function to quantize DCT coefficients
std::vector<int> quantize_dct_coefficients(
    const vector<vector<double>>& dct_blocks) {
    double scaleFactor = 100.0;  // Adjust the scale factor
    int quantizationLevels =
        pow(2, Options::quantizationLevels);  // Number of quantization levels
    std::vector<int> quantizedCoefficients;
    for(const std::vector<double>& dctCoefficients: dct_blocks){
        for (const double coefficient : dctCoefficients) {
            // Apply quantization and round to the nearest integer
            int quantizedCoefficient =
                static_cast<int>(round(coefficient * scaleFactor));

            // Ensure that the quantized coefficient is within the defined range
            quantizedCoefficient =
                std::max(quantizedCoefficient, -quantizationLevels);
            quantizedCoefficient =
                std::min(quantizedCoefficient, quantizationLevels);

            quantizedCoefficients.push_back(quantizedCoefficient);
        }
    }

    return quantizedCoefficients;
}

// Function to encode a block of audio
vector<int> encode_block(const vector<short>& inputSamples) {
    // Apply DCT to the audioBlock
    vector<vector<double>> dctCoefficients = apply_dct(inputSamples);

    // Quantize the DCT coefficients and returns the results
    return quantize_dct_coefficients(dctCoefficients);
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
        cout << "This program can only process music in mono channel, do you "
                "wish to transform it? (y/n): ";
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
    clock_t startTime = clock();

    /* Check .wav files */
    SndfileHandle sfhIn{Options::musicName};
    if (check_wav_file(sfhIn) < 0)
        return 1;

    // Code beyond here is experimental

    BitStream outputBitStream{'w', Options::encodedName};

    size_t nChannels{static_cast<size_t>(sfhIn.channels())};  // must be 1
    size_t nFrames{static_cast<size_t>(sfhIn.frames())};

    std::vector<short> inputSamples(nChannels * nFrames);
    sfhIn.readf(inputSamples.data(), nFrames);

    size_t nBlocks{static_cast<size_t>(
        ceil(static_cast<double>(nFrames) / Options::blockSize))};

    // Do zero padding, if necessary
    inputSamples.resize(nBlocks * Options::blockSize * nChannels);

    create_file_header(sfhIn);

    // Vector for holding all DCT coefficients, channel by channel
    vector<vector<double>> x_dct(nChannels,
                                 vector<double>(nBlocks * Options::blockSize));

    // Vector for holding DCT computations
    vector<double> x(Options::blockSize);

    // Direct DCT
    fftw_plan plan_d = fftw_plan_r2r_1d(Options::blockSize, x.data(), x.data(),
                                        FFTW_REDFT10, FFTW_ESTIMATE);
    for (size_t n = 0; n < nBlocks; n++) {
        for (size_t c = 0; c < nChannels; c++) {
            for (size_t k = 0; k < Options::blockSize; k++)
                x[k] =
                    inputSamples[(n * Options::blockSize + k) * nChannels + c];

            fftw_execute(plan_d);
            // Keep only "dctFrac" of the "low frequency" coefficients
            for (size_t k = 0; k < Options::blockSize * Options::dctFrac; k++)
                x_dct[c][n * Options::blockSize + k] =
                    x[k] / (Options::blockSize << 1);
        }
    }

    std::vector<int> quantizedCoefficients = quantize_dct_coefficients(x_dct);

    for(const int sample: quantizedCoefficients)
        outputBitStream.writeNBits(sample, Options::quantizationLevels);

    outputBitStream.~BitStream();

    clock_t endTime = clock();

    std::cout << "Program took "
              << (double(endTime - startTime) / CLOCKS_PER_SEC) * 1000
              << " ms to run. Music compressed to " << Options::encodedName
              << std::endl;

    return 0;
}