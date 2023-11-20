#include <golomb_codec.h>
#include <string.h>
#include <cmath>
#include <iostream>
#include <sndfile.hh>
#include <vector>

using namespace std;

// configurable parameters
namespace Options {
string musicName = "../songs/sample01.wav";
string encodedName = "encodedSample";
size_t blockSize = 1024;
size_t quantizationBits = 8;  // bits to be discarded
bool lossy = false;
size_t nChannels;
size_t nFrames;
size_t sampleRate;
}  // namespace Options

static void print_usage() {
    cerr << "Usage: %s [OPTIONS]\n"
            "  OPTIONS:\n"
            "  -h, --help        --- print this help\n"
            "  -i, --input       --- set music file name (default: "
            "../songs/sample01.wav)\n"
            "  -o, --output      --- set encoded file name (default: "
            "encodedSample)\n"
            "  -b, --blockSize   --- set block size (default: 1024)\n"
            "  -q, --quant       --- set Quantization Levels (default: "
            "8)\n"
            "  -l, --lossy       --- set lossy compression (default: off)"
            "  -m, --modulus     --- set m number (default: automatic "
            "calculation)"
         << endl;
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
        } else if (strcmp(argv[i], "-q") == 0 ||
                   strcmp(argv[i], "--quant") == 0) {
            i++;
            if (i < argc && isdigit(*argv[i])) {
                Options::quantizationBits = atoi(argv[i]);
            } else {
                std::cerr
                    << "Error: Missing or bad argument for -l/--levels option."
                    << argv[i] << std::endl;
                return -1;
            }
        } else if (strcmp(argv[i], "-l") == 0 ||
                   strcmp(argv[i], "--lossy") == 0) {
            Options::lossy = true;
        } else if (argv[i][0] == '-') {
            std::cerr << "Error: Unknown option or argument: " << argv[i]
                      << std::endl;
            return -1;
        }
    }
    return 0;
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

int main(int argc, char* argv[]) {
    int ret = process_arguments(argc, argv);
    if (ret < 0)
        return 1;
    else if (ret > 0)
        return 0;

    clock_t startTime = clock();

    /* Check .wav file */
    SndfileHandle sfhIn{Options::musicName};
    if (check_wav_file(sfhIn) < 0)
        return 1;

    // Set Options variables
    Options::sampleRate = static_cast<size_t>(sfhIn.samplerate());
    Options::nChannels = static_cast<size_t>(sfhIn.channels());
    Options::nFrames = static_cast<size_t>(sfhIn.frames());

    // Create Golomb Encoder class
    GEncoder gEncoder;



    clock_t endTime = clock();
    std::cout << "Program took "
              << (double(endTime - startTime) / CLOCKS_PER_SEC) * 1000
              << " ms to run. Music compressed to " << Options::musicName
              << std::endl;

    return 0;
}