#include <image_codec.h>
#include <string.h>
#include <cmath>
#include <iostream>
#include <sndfile.hh>
#include <vector>

using namespace std;
using namespace cv;

// configurable parameters
namespace Options {
string imageName = "decodedImage.pgm";
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
                Options::imageName = argv[i];
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
    cout << "\nMovie Processing information: \n"
         << " - Encoded File Name: " << Options::encodedName
         << "\n - Decoded Music Name: " << Options::imageName
         << "\n - File Type: " << f.type
         << "\n - Block Size: " << f.blockSize
         << "\n - Image Width: " << f.width
         << "\n - Image Height: " << f.height
         // << "\n - Image Max Gray Level: " << f.maxGrayLevel
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

    std::cout << "Decoding file with " << unsigned(f.nFrames) << " Frames..."
              << endl;

    Mat decodedImg = gDecoder.decode_frame(0);

    // Write decoded samples to file
    imwrite(Options::imageName, decodedImg);

    std::cout << "\nImage read and decoded\n" << endl;

    clock_t endTime = clock();
    std::cout << "Program took "
              << (double(endTime - startTime) / CLOCKS_PER_SEC) * 1000
              << " ms to run. Movie decompressed to " << Options::imageName
              << std::endl;

    return 0;
}