#include <frame.h>
#include <image_codec.h>
#include <string.h>
#include <cmath>
#include <iostream>
#include <sndfile.hh>
#include <vector>

using namespace std;

// configurable parameters
namespace Options {
string movieName = "decodedMovie.y4m";
string encodedName = "encodedMovie";
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
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--input") == 0) {
            i++;
            if (i < argc) {
                Options::encodedName = argv[i];
            } else {
                std::cerr << "Error: Missing argument for -i/--input option." << std::endl;
                return -1;
            }
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            i++;
            if (i < argc) {
                Options::movieName = argv[i];
            } else {
                std::cerr << "Error: Missing argument for -o/--output option." << std::endl;
                return -1;
            }
        } else if (argv[i][0] == '-') {
            std::cerr << "Error: Unknown option or argument: " << argv[i] << std::endl;
            return -1;
        }
    }
    return 0;
}

void print_processing_information(File& f, int nBlocks) {
    cout << "\nMovie Processing information: \n"
         << " - Encoded File Name: " << Options::encodedName
         << "\n - Decoded Music Name: " << Options::movieName << "\n - File Type: " << f.type
         << "\n - Block Size: " << f.blockSize << "\n - Chroma Value: " << f.chroma
         << "\n - Number of Frames: " << f.nFrames << "\n - Frames Width: " << f.width
         << "\n - Frames Height: " << f.height << "\n - Frames per Second: " << f.fps
         << "\n - Number of Blocks per Frame: " << nBlocks
         << "\n - Golomb Approach: " << approach_to_string(f.approach)
         << "\n - Encode type: " << (f.lossy ? "lossy" : "lossless") << "\n"
         << endl;
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

    File f = gDecoder.get_file();
    print_processing_information(f, nBlocks);

    std::cout << "Video Decoding starting..." << endl;

    for (int fId = 1; fId <= (int)f.nFrames; fId++) {
        Mat decodedFrame = gDecoder.decode_frame(fId);
        Frame::display_image(decodedFrame);
    }

    std::cout << "\nAll Frames read and decoded. All good!\n" << endl;

    clock_t endTime = clock();
    std::cout << "Program took " << (double(endTime - startTime) / CLOCKS_PER_SEC) * 1000
              << " ms to run. Movie decompressed to " << Options::movieName << std::endl;

    return 0;
}