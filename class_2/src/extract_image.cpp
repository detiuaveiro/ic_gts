#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;
using namespace std;

namespace Options {
String inputFile = "../images/airplane.ppm";
String outputFile = "extractedColor.ppm";
uint8_t channel = 1;
bool showNewImage = false;
}

static void print_usage() {
    cerr << "Usage: %s [OPTIONS]\n"
            "  OPTIONS:\n"
            "  -h, --help        --- print this help\n"
            "  -i, --input       --- set image file name (default: "
            "../images/airplane.ppm)\n"
            "  -o, --output      --- set extracted image file name (default: "
            "extractedColor.ppm)\n"
            "  -c, --channel     --- set channel number, [default] 1 (Blue), 2 "
                "(Green), or 3 (Red)\n"
            "  -s, --show        --- flag to show extracted image\n"
         << endl;
}

bool check_ppm_extension(const std::string& str) {
    size_t found = str.find(".ppm");

    if (found != std::string::npos) {  // ".ppm" is found
        // Check if it's at the end of the string or if it's a file extension
        if (found == str.length() - 4) {
            return true;
        }
    }
    return false;
}

int parse_args(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {  // Start for at 1, to skip program name
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage();
            return 1;
        } else if (strcmp(argv[i], "-i") == 0 ||
                   strcmp(argv[i], "--input") == 0) {
            i++;
            if (i < argc) {
                if (!check_ppm_extension(argv[i])) {
                    std::cerr
                        << "Error: .ppm file extension needed for input option"
                        << std::endl;
                    return -1;
                }
                Options::inputFile = argv[i];
            } else {
                std::cerr << "Error: Missing argument for -i/--input option."
                          << std::endl;
                return -1;
            }
        } else if (strcmp(argv[i], "-o") == 0 ||
                   strcmp(argv[i], "--output") == 0) {
            i++;
            if (i < argc) {
                if (!check_ppm_extension(argv[i])) {
                    std::cerr
                        << "Error: .ppm file extension needed for output option"
                        << std::endl;
                    return -1;
                }
                Options::outputFile = argv[i];
            } else {
                std::cerr << "Error: Missing argument for -o/--output option."
                          << std::endl;
                return -1;
            }
        } else if (strcmp(argv[i], "-c") == 0 ||
                   strcmp(argv[i], "--channel") == 0) {
            i++;
            if (i < argc && isdigit(*argv[i])) {
                Options::channel = atoi(argv[i]);
            } else {
                std::cerr << "Error: Missing or bad argument for "
                             "-c/--channel option: "
                          << argv[i] << std::endl;
                return -1;
            }
        } else if (strcmp(argv[i], "-s") == 0 ||
                   strcmp(argv[i], "--show") == 0) {
            Options::showNewImage = true;
        } else if (argv[i][0] == '-') {
            std::cerr << "Error: Unknown option or argument: " << argv[i]
                      << std::endl;
            return -1;
        }
    }
    return 0;
}

// Extract a color channel from an image, creating a single channel image with the result
int main(int argc, char** argv) {
    clock_t startTime = clock();

    int ret = parse_args(argc, argv);
    if (ret < 0)
        return 1;
    else if (ret > 0)
        return 0;

    if (Options::channel < 1 || Options::channel > 3) {
        cerr << "Error: channel number must be between [1-3]: 1 (Blue), 2 "
                "(Green), or 3 (Red)\n";
        return 1;
    }
    Options::channel--;  // adjust for channel starting at 0

    Mat img = imread(Options::inputFile);
    if (img.empty()) {
        cerr << "Error: could not open/find image\n";
        return 1;
    }

    // Create a "empty" image
    cv::Mat extractedChannel = cv::Mat::zeros(img.size(), CV_8UC3);

    // Split the image into individual channels
    std::vector<cv::Mat> channels;
    cv::split(img, channels);

    // Set the extracted channel and leave the others as zeros
    // 1 line for all values of a given channel
    std::vector<cv::Mat> newChannels(
        3, cv::Mat(img.size(), CV_8UC1, cv::Scalar(0)));

    // only assign values to the selected channel
    newChannels[Options::channel] = channels[Options::channel];

    // merge the channels back into a single image
    cv::merge(newChannels, extractedChannel);

    // Save the result to the specified output file (obligatory .ppm)
    cv::imwrite(Options::outputFile, extractedChannel);

    clock_t endTime = clock();

    std::cout << "Program took "
              << (double(endTime - startTime) / CLOCKS_PER_SEC) * 1000
              << " ms to extract Color channel " << Options::channel
              << " save image as " << Options::outputFile << std::endl;

    imshow("Extracted Color Channel", extractedChannel);
    waitKey(0); // wait for any key to be pressed

    return 0;
}