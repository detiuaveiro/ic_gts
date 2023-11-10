#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;
using namespace std;

namespace Options {
String inputFile = "../images/peppers.ppm";
String outputFile = "extractedColor.ppm";
uint8_t channel = 1;
bool showNewImage = false;
}  // namespace Options

static void print_usage() {
    cerr << "Usage: %s [OPTIONS]\n"
            "  OPTIONS:\n"
            "  -h, --help        --- print this help\n"
            "  -i, --input       --- set image file name (default: "
            "../images/peppers.ppm)\n"
            "  -o, --output      --- set extracted image file name (default: "
            "extractedColor.ppm)\n"
            "  -c, --channel     --- set channel number, [default] 1 (Blue), 2 "
            "(Green), or 3 (Red)\n"
            "  -s, --show        --- flag to show extracted image\n"
         << endl;
}

bool check_image_extension(const std::string& filename) {
    // List of supported image extensions
    std::vector<std::string> supportedExtensions = {".png", ".jpg",  ".jpeg",
                                                    ".bmp", ".tiff", ".ppm"};

    // Convert the filename to lowercase for case-insensitive comparison
    std::string lowercaseFilename = filename;
    std::transform(lowercaseFilename.begin(), lowercaseFilename.end(),
                   lowercaseFilename.begin(), ::tolower);

    // Check if the filename has any of the supported extensions
    for (const std::string& extension : supportedExtensions) {
        if (lowercaseFilename.length() >= extension.length() &&
            lowercaseFilename.compare(
                lowercaseFilename.length() - extension.length(),
                extension.length(), extension) == 0) {
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
                if (!check_image_extension(argv[i])) {
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
                if (!check_image_extension(argv[i])) {
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

    // Create a single-channel image with the specified channel
    cv::Mat outputImage = cv::Mat::zeros(img.rows, img.cols, CV_8UC1);

    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            /* Since outputImage is single-channel, each pixel is represented 
                by an unsigned char. However, considering that the input image 
                is a 3-channel image, we use the Vec3b vector class that already 
                holds the 3 channels, with each being an 8-bit unsigned char that's
                why we use [] to index the correct channel */
            outputImage.at<uchar>(i, j) =
                img.at<cv::Vec3b>(i, j)[Options::channel];
        }
    }

    // Since it's expected a image with one channel, will have to convert it to
    //  grayscale
    cv::Mat outputGray;
    cv::cvtColor(outputImage, outputGray, cv::COLOR_GRAY2BGR);

    // Save the result to the output file
    cv::imwrite(Options::outputFile, outputGray);

    clock_t endTime = clock();

    std::cout << "Program took "
              << (double(endTime - startTime) / CLOCKS_PER_SEC) * 1000
              << " ms to extract Color channel " << Options::channel
              << " save image as " << Options::outputFile << std::endl;

    if (Options::showNewImage) {
        imshow("Extracted Color Channel", outputImage);
        waitKey(0);
    }

    return 0;
}