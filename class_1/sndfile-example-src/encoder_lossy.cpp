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
string encodedName = "encodedSample";
size_t blockSize = 1024;
size_t quantizationLevels = 8;  // or 256 levels
double dctFrac = 0.2;
size_t nChannels;  // should be always 1, but will leave it for future upgrades
size_t nFrames;
size_t sampleRate;
}  // namespace Options

static void print_usage() {
    cerr
        << "Usage: %s [OPTIONS]\n"
           "  OPTIONS:\n"
           "  -h, --help        --- print this help\n"
           "  -i, --input       --- set music file name (default: sample.wav)\n"
           "  -o, --output      --- set encoded file name (default: "
           "encodedSample)\n"
           "  -b, --blockSize   --- set block size (default: 1024)\n"
           "  -l, --levels      --- set Quantization Levels (default: 8)\n"
           "  -d, --dctFrac     --- set Dct Frac (default: 0.2)\n"
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
            if (i < argc) {
                Options::musicName = argv[i];
            } else {
                std::cerr << "Error: Missing argument for -i/--input option."
                          << std::endl;
                return -1;
            }
        } else if (strcmp(argv[i], "-o") == 0 ||
                   strcmp(argv[i], "--output") == 0) {
            i++;
            if (i < argc) {
                Options::encodedName = argv[i];
            } else {
                std::cerr << "Error: Missing argument for -o/--output option."
                          << std::endl;
                return -1;
            }
        } else if (strcmp(argv[i], "-b") == 0 ||
                   strcmp(argv[i], "--blockSize") == 0) {
            i++;
            if (i < argc && isdigit(*argv[i])) {
                Options::blockSize = atoi(argv[i]);
            } else {
                std::cerr << "Error: Missing or bad argument for "
                             "-b/--blockSize option: "
                          << argv[i] << std::endl;
                return -1;
            }
        } else if (strcmp(argv[i], "-l") == 0 ||
                   strcmp(argv[i], "--levels") == 0) {
            i++;
            if (i < argc && isdigit(*argv[i])) {
                Options::quantizationLevels = atoi(argv[i]);
            } else {
                std::cerr
                    << "Error: Missing or bad argument for -l/--levels option."
                    << argv[i] << std::endl;
                return -1;
            }
        } else if (strcmp(argv[i], "-d") == 0 ||
                   strcmp(argv[i], "--dctFrac") == 0) {
            i++;
            if (i < argc && isdigit(*argv[i])) {
                Options::dctFrac = atof(argv[i]);
            } else {
                std::cerr
                    << "Error: Missing or bad argument for -d/--dctFrac option."
                    << argv[i] << std::endl;
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

void print_processing_information(int nBlocks) {
    cout << "\nMusic Processing information: \n"
         << " - Music File Name: " << Options::musicName
         << "\n - Encoded File Name: " << Options::encodedName
         << "\n - Block Size: " << Options::blockSize
         << "\n - Number of Channels: " << Options::nChannels
         << "\n - Sample Rate: " << Options::sampleRate
         << "\n - Quantization Levels: " << Options::quantizationLevels
         << "\n - Dct Frac: " << Options::dctFrac
         << "\n - Total Number of Frames: " << Options::nFrames
         << "\n - Number of Blocks: " << nBlocks << "\n"
         << endl;
}

/* 
    Because of this "The decoder should rely only on the (binary) file 
        produced by the encoder in order to reconstruct (an approximate 
        version of) the audio", we need to include a sort of header in the 
        file with all the parameters 
*/
void create_file_header(BitStream& outputStream) {
    // Store Block Size (16 bits)
    outputStream.writeNBits(Options::blockSize, 16);

    // Store Number of channels -> technically should be always 1 (3 bits)
    outputStream.writeNBits(Options::nChannels, 3);

    // Store SampleRate (16 bits)
    outputStream.writeNBits(Options::sampleRate, 16);

    // Store Quantization Levels (16 bits)
    outputStream.writeNBits(Options::quantizationLevels, 16);

    // Store DCT Fraction (7 bits > max 100)
    outputStream.writeNBits(int(Options::dctFrac * 100), 7);

    // Store Total Number of frames (32 bits)
    outputStream.writeNBits(Options::nFrames, 32);
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

// Function to quantize DCT coefficients
std::vector<int> quantize_dct_coefficients(
    const vector<vector<double>>& dct_blocks) {

    uint cutBits;
    if (Options::quantizationLevels <= 16)
        cutBits = 16 - Options::quantizationLevels;
    else
        cutBits = 32 - Options::quantizationLevels;
    std::vector<int> quantizedCoefficients;
    for (const std::vector<double>& dctCoefficients : dct_blocks) {
        for (const double coefficient : dctCoefficients) {
            int quantizedCoefficient = int(coefficient);
            quantizedCoefficient >>= cutBits;  // LSB at 0
            quantizedCoefficient = quantizedCoefficient
                                   << cutBits;  // Revert the position change
            quantizedCoefficients.push_back(quantizedCoefficient);
        }
    }

    return quantizedCoefficients;
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

    BitStream outputBitStream{'w', Options::encodedName};

    Options::sampleRate = static_cast<size_t>(sfhIn.samplerate());
    Options::nChannels = static_cast<size_t>(sfhIn.channels());  // must be 1
    Options::nFrames = static_cast<size_t>(sfhIn.frames());

    std::vector<short> inputSamples(Options::nChannels * Options::nFrames);
    sfhIn.readf(inputSamples.data(), Options::nFrames);

    size_t nBlocks{static_cast<size_t>(
        ceil(static_cast<double>(Options::nFrames) / Options::blockSize))};

    // Do zero padding, if necessary
    inputSamples.resize(nBlocks * Options::blockSize * Options::nChannels);

    print_processing_information(nBlocks);
    create_file_header(outputBitStream);

    // Vector for holding all DCT coefficients, channel by channel
    vector<vector<double>> x_dct(Options::nChannels,
                                 vector<double>(nBlocks * Options::blockSize));

    // Vector for holding DCT computations
    vector<double> x(Options::blockSize);

    // Direct DCT
    fftw_plan plan_d = fftw_plan_r2r_1d(Options::blockSize, x.data(), x.data(),
                                        FFTW_REDFT10, FFTW_ESTIMATE);
    for (size_t n = 0; n < nBlocks; n++) {
        for (size_t c = 0; c < Options::nChannels; c++) {
            for (size_t k = 0; k < Options::blockSize; k++)
                x[k] = inputSamples[(n * Options::blockSize + k) *
                                        Options::nChannels +
                                    c];

            fftw_execute(plan_d);
            // Keep only "dctFrac" of the "low frequency" coefficients
            for (size_t k = 0; k < Options::blockSize * Options::dctFrac; k++)
                x_dct[c][n * Options::blockSize + k] =
                    x[k] / (Options::blockSize << 1);
        }
    }

    std::vector<int> quantizedCoefficients = quantize_dct_coefficients(x_dct);

    std::cout << "Number of quantized coefficients: "
              << quantizedCoefficients.size() << std::endl;

    for (int sample : quantizedCoefficients)
        outputBitStream.writeNBits(sample, Options::quantizationLevels);

    outputBitStream.~BitStream();

    // Destroy the allocated data
    fftw_destroy_plan(plan_d);
    fftw_cleanup();

    clock_t endTime = clock();

    std::cout << "Program took "
              << (double(endTime - startTime) / CLOCKS_PER_SEC) * 1000
              << " ms to run. Music compressed to " << Options::musicName
              << std::endl;

    return 0;
}