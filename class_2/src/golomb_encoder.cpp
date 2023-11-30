#include <golomb_codec.h>
#include <string.h>
#include <cmath>
#include <iostream>
#include <sndfile.hh>
#include <vector>

using namespace std;

// configurable parameters
namespace Options {
string musicName = "../songs/sample_mono.wav";
string encodedName = "encodedSample";
size_t blockSize = 1024;
size_t bitRate = 8;
int m = -1;  // automatic
bool lossy = false;
size_t nChannels;
size_t nFrames;
size_t sampleRate;
PREDICTOR_TYPE predictor = AUTOMATIC;
APPROACH approach = SIGN_MAGNITUDE;
PHASE phase = P_AUTOMATIC;
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
            "  -b, --bitRate     --- set bit rate (default: 8)\n"
            "  -l, --lossy       --- set lossy compression (default: off)\n"
            "  -m, --modulus     --- set m number (default: automatic "
            "calculation)\n"
            "  -p, --predict     --- set predictor [0,3] (default: PREDICT1)\n"
            "  -a, --approach    --- set approach [0,1] (default: "
            "SIGN_MAGNITUDE)"
            "  -s, --phase       --- set phase [0,3], only applicable to "
            "stereo audio (default: P_AUTOMATIC)"
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
        } else if (strcmp(argv[i], "-b") == 0 ||
                   strcmp(argv[i], "--bitRate") == 0) {
            i++;
            if (i < argc && isdigit(*argv[i])) {
                Options::bitRate = atoi(argv[i]);
            } else {
                std::cerr
                    << "Error: Missing or bad argument for -l/--levels option."
                    << argv[i] << std::endl;
                return -1;
            }
        } else if (strcmp(argv[i], "-l") == 0 ||
                   strcmp(argv[i], "--lossy") == 0) {
            Options::lossy = true;
        } else if (strcmp(argv[i], "-m") == 0 ||
                   strcmp(argv[i], "--modulus") == 0) {
            i++;
            if (i < argc && isdigit(*argv[i])) {
                Options::m = atoi(argv[i]);
            } else {
                std::cerr
                    << "Error: Missing or bad argument for -m/--modulus option."
                    << argv[i] << std::endl;
                return -1;
            }
        } else if (strcmp(argv[i], "-p") == 0 ||
                   strcmp(argv[i], "--predict") == 0) {
            i++;
            if (i < argc && isdigit(*argv[i])) {
                Options::predictor = static_cast<PREDICTOR_TYPE>(atoi(argv[i]));
            } else {
                std::cerr
                    << "Error: Missing or bad argument for -p/--predict option."
                    << argv[i] << std::endl;
                return -1;
            }
        } else if (strcmp(argv[i], "-a") == 0 ||
                   strcmp(argv[i], "--approach") == 0) {
            i++;
            if (i < argc && isdigit(*argv[i])) {
                Options::approach = static_cast<APPROACH>(atoi(argv[i]));
            } else {
                std::cerr
                    << "Error: Missing or bad argument for -p/--predict option."
                    << argv[i] << std::endl;
                return -1;
            }
        } else if (strcmp(argv[i], "-s") == 0 ||
                   strcmp(argv[i], "--phase") == 0) {
            i++;
            if (i < argc && isdigit(*argv[i])) {
                Options::phase = static_cast<PHASE>(atoi(argv[i]));
            } else {
                std::cerr
                    << "Error: Missing or bad argument for -s/--phase option."
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

void print_processing_information(int nBlocks) {
    cout << "\nMusic Processing information: \n"
         << " - Music File Name: " << Options::musicName
         << "\n - Encoded File Name: " << Options::encodedName
         << "\n - Block Size: " << Options::blockSize
         << "\n - Number of Channels: " << Options::nChannels
         << "\n - Sample Rate: " << Options::sampleRate
         << "\n - Total Number of Frames: " << Options::nFrames
         << "\n - Number of Blocks: " << nBlocks
         << "\n - Predictor: " << get_type_string(Options::predictor)
         << "\n - Golomb Approach: " << approach_to_string(Options::approach)
         << "\n - Phase: " << get_phase_string(Options::phase)
         << "\n - Encode type: " << (Options::lossy ? "lossy" : "lossless")
         << "\n"
         << endl;
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

    if (!check_approach(Options::approach)) {
        cerr << "Error: Invalid approach type " << Options::approach << endl;
        return 1;
    }

    // Set Options variables
    Options::sampleRate = static_cast<size_t>(sfhIn.samplerate());
    Options::nChannels = static_cast<size_t>(sfhIn.channels());
    Options::nFrames = static_cast<size_t>(sfhIn.frames());

    std::vector<short> inputSamples(Options::nChannels * Options::nFrames);
    sfhIn.readf(inputSamples.data(), Options::nFrames);

    size_t nBlocks{static_cast<size_t>(
        ceil(static_cast<double>(Options::nFrames) / Options::blockSize))};

    // Do zero padding, if necessary
    inputSamples.resize(nBlocks * Options::blockSize * Options::nChannels);

    print_processing_information(nBlocks);

    // if lossy show current bit rate and ask for new bit rate

    // Create Golomb Encoder class
    GEncoder gEncoder(Options::encodedName, Options::m, Options::predictor,
                      Options::phase);

    // Create file struct
    File f;
    f.sampleRate = Options::sampleRate;
    f.blockSize = Options::blockSize;
    f.nChannels = Options::nChannels;
    f.nFrames = Options::nFrames;
    f.blocks = std::vector<Block>();
    f.bitRate = Options::bitRate;
    f.lossy = Options::lossy;
    f.approach = Options::approach;

    // dont forget to multiply the number of blocks by the number of channels
    //  since each the nBlocks is accounting for raw frames and in the case of
    //  stereo audio, each frame has two samples, so double the calculated blocks
    //  (because blocks have samples, not frames)
    gEncoder.encode_file(f, inputSamples, nBlocks * Options::nChannels);

    clock_t endTime = clock();
    std::cout << "Program took " << std::fixed << std::setprecision(2)
              << (double(endTime - startTime) / CLOCKS_PER_SEC)
              << " seconds to run. Music compressed to " << Options::encodedName
              << std::endl;

    return 0;
}