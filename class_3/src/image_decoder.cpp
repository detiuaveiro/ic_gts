#include <audio_codec.h>
#include <string.h>
#include <cmath>
#include <iostream>
#include <sndfile.hh>
#include <vector>

using namespace std;

// configurable parameters
namespace Options {
string musicName = "decodedMusic.wav";
string encodedName = "encodedSample";
}  // namespace Options

static void print_usage() {
    cerr << "Usage: %s [OPTIONS]\n"
            "  OPTIONS:\n"
            "  -h, --help        --- print this help\n"
            "  -i, --input       --- set encoded file name (default: "
            "encodedSample)\n"
            "  -o, --output      --- set decoded music name (default: "
            "decodedMusic.wav)\n"
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
                Options::encodedName = argv[i];
            } else {
                std::cerr << "Error: Missing argument for -i/--input option."
                          << std::endl;
                return -1;
            }
        } else if (strcmp(argv[i], "-o") == 0 ||
                   strcmp(argv[i], "--output") == 0) {
            i++;
            if (i < argc) {
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

void print_processing_information(File& f, int nBlocks) {
    cout << "\nMusic Processing information: \n"
         << " - Encoded File Name: " << Options::encodedName
         << "\n - Decoded Music Name: " << Options::musicName
         << "\n - Block Size: " << f.blockSize
         << "\n - Number of Channels: " << unsigned(f.nChannels)
         << "\n - Sample Rate: " << f.sampleRate
         << "\n - Total Number of Frames: " << unsigned(f.nFrames)
         << "\n - Number of Blocks: " << nBlocks
         << "\n - Golomb Approach: " << approach_to_string(f.approach)
         << "\n - Encode type: " << (f.lossy ? "lossy" : "lossless") << "\n"
         << endl;
}

void save_decoded_music(File& f, std::vector<short>& samples) {
    SndfileHandle sfhOut{
        Options::musicName, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_16,
        static_cast<int>(f.nChannels), static_cast<int>(f.sampleRate)};
    if (sfhOut.error()) {
        cerr << "Error: problem generating output .wav file\n";
        exit(1);
    }

    sfhOut.writef(samples.data(), samples.size() / f.nChannels);
}

int main(int argc, char* argv[]) {
    int ret = process_arguments(argc, argv);
    if (ret < 0)
        return 1;
    else if (ret > 0)
        return 0;

    clock_t startTime = clock();

    // Create Golomb Encoder class
    GDecoder gDecoder(Options::encodedName);

    int nBlocks = gDecoder.read_file_header();

    print_processing_information(gDecoder.get_file(), nBlocks);

    std::vector<short> decodedSamples = gDecoder.decode_file();

    save_decoded_music(gDecoder.get_file(), decodedSamples);

    clock_t endTime = clock();
    std::cout << "Program took "
              << (double(endTime - startTime) / CLOCKS_PER_SEC) * 1000
              << " ms to run. Music decompressed to " << Options::musicName
              << std::endl;

    return 0;
}