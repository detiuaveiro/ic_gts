#include <image_codec.h>
#include <movie.h>
#include <string.h>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

// either read an image or a video
// if image, then nFrames = 1
// -> cols and rows are the dimensions of the image

// recebo um ficheiro y4m ou pgm ->
// 1. se for y4m, criar um objeto da classe movie -> read frame from movie -> dps usar classe frame -> encoding
// 2. se for pgm, criar um objeto da classe frame -> encoding

// configurable parameters
namespace Options {
string fileName = "../movies/sintel_trailer_2k_480p24.y4m";
string encodedName = "encodedMovie";
size_t blockSize = 64;
size_t nFrames;
int m = -1;  // automatic
bool lossy = false;
PREDICTOR_TYPE predictor = AUTOMATIC;
APPROACH approach = SIGN_MAGNITUDE;
int intraFramePeriodicity = 10;
}  // namespace Options

static void print_usage() {
    cerr
        << "Usage: %s [OPTIONS]\n"
           "  OPTIONS:\n"
           "  -h, --help        --- print this help\n"
           "  -i, --input       --- set image/video file name (default: "
           "../images/airplane_gray.pgm)\n"
           "  -o, --output      --- set encoded file name (default: "
           "encodedMovie)\n"
           "  -b, --blockSize   --- set block size (default: 1024)\n"
           "  -l, --lossy       --- set lossy compression (default: off)\n"
           "  -m, --modulus     --- set m number (default: automatic "
           "calculation)\n"
           "  -p, --predict     --- set predictor [0,7] (default: JPEG1)\n"
           "  -f, --framePeriod --- set intra frame periodicity (default: 10)\n"
           "  -a, --approach    --- set approach [0,1] (default: "
           "SIGN_MAGNITUDE)"
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
                Options::fileName = argv[i];
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
        } else if (strcmp(argv[i], "-f") == 0 ||
                   strcmp(argv[i], "--framePeriod") == 0) {
            i++;
            if (i < argc && isdigit(*argv[i])) {
                Options::intraFramePeriodicity =
                    static_cast<int>(atoi(argv[i]));
            } else {
                std::cerr << "Error: Missing or bad argument for "
                             "-f/--framePeriod option."
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
    cout << "\nVideo Processing information: \n"
         << " - Video File Name: " << Options::fileName
         << "\n - Encoded File Name: " << Options::encodedName
         << "\n - Block Size: " << Options::blockSize
         << "\n - Total Number of Frames: " << Options::nFrames
         << "\n - Number of Blocks per Frame: " << nBlocks
         << "\n - Predictor: " << get_type_string(Options::predictor)
         << "\n - Golomb Approach: " << approach_to_string(Options::approach)
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

    // TODO - Check .pgm or .y4m file
    Movie movieClass{};
    fstream movieStream(Options::fileName, std::ios::in);

    // Create Golomb Encoder class
    GEncoder gEncoder(Options::encodedName, Options::m, Options::predictor);

    movieClass.getHeaderParameters(movieStream);

    File f;
    f.type = Y4M;
    f.blockSize = Options::blockSize;
    f.nFrames = movieClass.getNumberFrames();
    f.chroma = movieClass.getChroma();
    f.width = movieClass.getWidth();
    f.height = movieClass.getHeight();
    f.fps = movieClass.getFps();
    f.approach = Options::approach;
    f.lossy = Options::lossy;

    int frameSize = static_cast<int>(MovieFile::width * MovieFile::height);

    size_t nBlocksPerFrame{static_cast<size_t>(
        ceil(static_cast<double>(frameSize) / Options::blockSize))};

    Options::nFrames = movieClass.getNumberFrames();

    print_processing_information(nBlocksPerFrame);

    gEncoder.encode_file_header(f, nBlocksPerFrame,
                                Options::intraFramePeriodicity);

    Mat mat = Mat();
    int frameCounter = 0;
    while (true) {
        Mat frame = movieClass.readFrameFromMovie(movieStream);
        if (frame.size() == mat.size())
            break;
        gEncoder.encode_frame(frame, nBlocksPerFrame);
        frameCounter++;
    }

    movieStream.close();

    clock_t endTime = clock();
    std::cout << "Program took " << std::fixed << std::setprecision(2)
              << (double(endTime - startTime) / CLOCKS_PER_SEC)
              << " seconds to run. Music compressed to " << Options::encodedName
              << std::endl;

    return 0;
}