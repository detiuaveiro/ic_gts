#include <frame.h>
#include <cmath>

uint8_t Frame::get_pixel(Mat& image, int pixelIndex) {
    uint8_t pixel = image.at<uint8_t>(pixelIndex);
    return pixel;
}

/*! \attention If one of the dimensions isn't multiple of the blockSize, 
    some blocks may end up with inferior size on that axis 
    To know the size of the of the last block, which will be cut, just divide
    the axis by blockSize */
std::vector<cv::Mat> Frame::get_blocks(Mat& image, int blockSize) {
    std::vector<cv::Mat> blocks;

    int rows = image.rows;
    int cols = image.cols;

    for (int y = 0; y < rows; y += blockSize) {
        for (int x = 0; x < cols; x += blockSize) {
            int blockWidth = std::min(blockSize, cols - x);
            int blockHeight = std::min(blockSize, rows - y);

            cv::Mat block = image(cv::Range(y, y + blockHeight),
                                  cv::Range(x, x + blockWidth))
                                .clone();
            blocks.push_back(block);
        }
    }

    return blocks;
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