#include <frame.h>
#include <cmath>

uint8_t Frame::get_pixel(Mat& image, int pixelIndex) {
    uint8_t pixel = image.at<uint8_t>(pixelIndex);
    return pixel;
}

/*! \attention This function applies padding */
std::vector<cv::Mat> Frame::get_blocks(Mat& image, int blockSize) {
    std::vector<cv::Mat> blocks;

    int rows = image.rows;
    int cols = image.cols;

    int paddedRows = ((rows + blockSize - 1) / blockSize) * blockSize;
    int paddedCols = ((cols + blockSize - 1) / blockSize) * blockSize;

    Mat paddedImage;
    copyMakeBorder(image, paddedImage, 0, paddedRows - rows, 0, paddedCols - cols, BORDER_CONSTANT,
                   Scalar(0));

    for (int y = 0; y < paddedRows; y += blockSize) {
        for (int x = 0; x < paddedCols; x += blockSize) {
            int blockWidth = std::min(blockSize, paddedCols - x);
            int blockHeight = std::min(blockSize, paddedRows - y);

            cv::Mat block =
                paddedImage(cv::Range(y, y + blockHeight), cv::Range(x, x + blockWidth)).clone();
            blocks.push_back(block);
        }
    }

    return blocks;
}

Mat Frame::compose_blocks(std::vector<cv::Mat> blocks, int blockSize, int rows, int cols) {
    Mat composedImage(rows, cols, blocks[0].type(), Scalar(0));

    int currentBlock = 0;
    for (int y = 0; y < rows; y += blockSize) {
        for (int x = 0; x < cols; x += blockSize) {
            int blockWidth = std::min(blockSize, cols - x);
            int blockHeight = std::min(blockSize, rows - y);

            Rect roi(x, y, blockWidth, blockHeight);

            // Adjust block size for the padded blocks
            if (x + blockWidth > cols) {
                blockWidth = cols - x;
            }
            if (y + blockHeight > rows) {
                blockHeight = rows - y;
            }

            blocks[currentBlock++]
                .cv::Mat::operator()(Rect(0, 0, blockWidth, blockHeight))
                .copyTo(composedImage(roi));
        }
    }

    return composedImage;
}

std::vector<uint8_t> Frame::mat_to_linear_vector(Mat& image) {
    std::vector<uint8_t> linearData;
    linearData.reserve(image.rows * image.cols);

    for (int i = 0; i < image.rows; ++i) {
        for (int j = 0; j < image.cols; ++j) {
            linearData.push_back(image.at<uint8_t>(i, j));
        }
    }
    return linearData;
}

Mat Frame::linear_vector_to_mat(std::vector<uint8_t> data, int rows, int cols) {
    // Creating a single-channel (grayscale) image
    cv::Mat image(rows, cols, CV_8UC1);

    int dataIdx = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            image.at<uint8_t>(i, j) = data[dataIdx++];
        }
    }
    return image;
}

void Frame::display_image(Mat& image) {
    if (image.empty()) {
        std::cerr << "Error: Frame is empty" << std::endl;
        return;
    }

    // No need
    // Split the YUV image into separate channels
    //std::vector<cv::Mat> channels;
    //cv::split(image, channels);

    // Create U and V channels filled with 127
    //cv::Mat uChannel = cv::Mat::ones(image.size(), CV_8UC1) * 127;
    //cv::Mat vChannel = cv::Mat::ones(image.size(), CV_8UC1) * 127;

    // Merge Y, U, and V channels to form the final image
    //std::vector<cv::Mat> mergedChannels = { channels[0], uChannel, vChannel };
    //cv::Mat finalImage;
    // cv::merge(mergedChannels, finalImage);

    // Convert the final image to grayscale
    //cv::cvtColor(finalImage, finalImage, cv::COLOR_YUV2GRAY_420);

    // Display the image
    cv::imshow("YUV Image with U and V set to 127", image);
    cv::waitKey(0); // Wait indefinitely for a key press
}
