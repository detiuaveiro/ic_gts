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
string encodedName = "encodedSample";
string musicName = "decodedSample.wav";
size_t blockSize;
size_t quantizationLevels;
double dctFrac;
size_t nChannels;  // should be always 1, but will leave it for future upgrades
size_t nFrames;
size_t sampleRate;
}  // namespace Options

static void print_usage() {
    cerr << "Usage: %s [OPTIONS]\n"
            "  OPTIONS:\n"
            "  -h, --help        --- print this help\n"
            "  -i, --input       --- set encoded file name (default: "
            "encodedSample)\n"
            "  -o, --output      --- set encoded file name (default: "
            "decodedSample.wav)\n"
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
        } else if (argv[i][0] == '-') {
            std::cerr << "Error: Unknown option or argument: " << argv[i]
                      << std::endl;
            return -1;
        }
    }
    return 0;
}

void print_processing_information(int nBlocks) {
    cout << "\nEncoded file Processing information: \n"
         << " - Encoded File Name: " << Options::encodedName
         << "\n - Decoded File Name: " << Options::musicName
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
void read_file_header(BitStream& inputStream) {
    // Store Block Size (16 bits)
    Options::blockSize = static_cast<size_t>(inputStream.readNBits(16));

    // Store Number of channels -> technically should be always 1 (3 bits)
    Options::nChannels = static_cast<size_t>(inputStream.readNBits(3));

    // Store SampleRate (16 bits)
    Options::sampleRate = static_cast<size_t>(inputStream.readNBits(16));

    // Store Quantization Levels (16 bits)
    Options::quantizationLevels =
        static_cast<size_t>(inputStream.readNBits(16));

    // Store DCT Fraction (7 bits > max 100)
    Options::dctFrac = double(inputStream.readNBits(7)) / 100.0;

    // Store Total Number of frames (32 bits)
    Options::nFrames = static_cast<size_t>(inputStream.readNBits(32));
}

int main(int argc, char* argv[]) {
    clock_t startTime = clock();

    int ret = process_arguments(argc, argv);
    if (ret < 0)
        return 1;
    else if (ret > 0)
        return 0;

    BitStream inputBitStream{'r', Options::encodedName};

    read_file_header(inputBitStream);

    size_t nBlocks{static_cast<size_t>(
        ceil(static_cast<double>(Options::nFrames) / Options::blockSize))};

    print_processing_information(nBlocks);

    SndfileHandle sfhOut{Options::musicName, SFM_WRITE,
                         SF_FORMAT_WAV | SF_FORMAT_PCM_16,
                         static_cast<int>(Options::nChannels),
                         static_cast<int>(Options::sampleRate)};
    if (sfhOut.error()) {
        cerr << "Error: problem generating output .wav file\n";
        return 1;
    }

    // Read all the values in the encoded file
    std::vector<int> quantizedCoefficients;

    while (!inputBitStream.check_eof())
        quantizedCoefficients.push_back(
            inputBitStream.readNBits(Options::quantizationLevels));

    std::cout << "Number of quantized coefficients: "
              << quantizedCoefficients.size() << endl;

    inputBitStream.~BitStream();

    // Vector for holding all DCT coefficients, channel by channel
    vector<vector<double>> x_dct(Options::nChannels,
                                 vector<double>(nBlocks * Options::blockSize));

    // Divide the coefficients vector into blocks
    int index = 0;
    for (size_t n = 0; n < nBlocks; n++) {
        for (size_t c = 0; c < Options::nChannels; c++) {
            for (size_t k = 0; k < Options::blockSize; k++) {
                x_dct[c][n * Options::blockSize + k] =
                    double(quantizedCoefficients[index++]) / 100.0;
            }
        }
    }

    // Vector for holding DCT computations
    vector<double> x(Options::blockSize);

    std::vector<short> outputSamples(Options::nChannels * Options::nFrames);
    // Do zero padding, if necessary
    outputSamples.resize(nBlocks * Options::blockSize *
                         Options::nChannels);  // not necessary?

    // Inverse DCT
    fftw_plan plan_i = fftw_plan_r2r_1d(Options::blockSize, x.data(), x.data(),
                                        FFTW_REDFT01, FFTW_ESTIMATE);
    for (size_t n = 0; n < nBlocks; n++)
        for (size_t c = 0; c < Options::nChannels; c++) {
            for (size_t k = 0; k < Options::blockSize; k++)
                x[k] = x_dct[c][n * Options::blockSize + k];

            fftw_execute(plan_i);
            for (size_t k = 0; k < Options::blockSize; k++)
                outputSamples[(n * Options::blockSize + k) *
                                  Options::nChannels +
                              c] = static_cast<short>(round(x[k]));
        }

    sfhOut.writef(outputSamples.data(), Options::nFrames);

    // Destroy the allocated data
    fftw_destroy_plan(plan_i);
    fftw_cleanup();

    clock_t endTime = clock();

    std::cout << "Program took "
              << (double(endTime - startTime) / CLOCKS_PER_SEC) * 1000
              << " ms to run. Music decompressed to " << Options::encodedName
              << std::endl;

    return 0;
}