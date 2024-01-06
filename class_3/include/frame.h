#pragma once

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

namespace Frame {
uint8_t get_pixel(Mat& image, int pixelIndex);

std::vector<cv::Mat> get_blocks(Mat& image, int blockSize);

Mat compose_blocks(std::vector<cv::Mat> blocks, int blockSize, int rows,
                   int cols);

std::vector<uint8_t> mat_to_linear_vector(Mat& image);

Mat linear_vector_to_mat(std::vector<uint8_t> data, int rows, int cols);

};  // namespace Frame
