// Program to compare two video sequences in terms of the peak signal to noise ratio (PSNR).

#include <iostream>
#include <string>
#include <vector>
#include <movie.h>
#include <opencv2/opencv.hpp>

using namespace std;

int main(int argc, char* argv[]) {

    // Ask the user for the original file and the reconstructed file
    if (argc < 3)
	{
		cerr << "Usage: " << argv[0] << " <original file> <reconstructed file>\n";
		return 1;
	}

    // Need to get width and height from the two videos
    // Use the MOVIE class to get the width and height of the videos
    Movie originalMovie{};
    Movie reconstructedMovie{};

    fstream originalFile;
    originalFile.open(argv[1], std::fstream::in | std::fstream::binary);
    fstream reconstructedFile;
    reconstructedFile.open(argv[2], std::fstream::in | std::fstream::binary);

    originalMovie.get_header_parameters(originalFile);
    reconstructedMovie.get_header_parameters(reconstructedFile);

    uint16_t width1 = originalMovie.get_width();
    uint16_t height1 = originalMovie.get_height();
    uint16_t width2 = reconstructedMovie.get_width();
    uint16_t height2 = reconstructedMovie.get_height();

    // Check if files have the same size
    if (width1 != width2 || height1 != height2){
        cerr << "Error: files have different sizes\n";
        return 1;
    }

    // TODO - Check if files have the same number of frames
    if (originalMovie.get_number_frames() != reconstructedMovie.get_number_frames()){
        cerr << "Error: files have different number of frames\n";
        return 1;
    }


    vector<uint8_t> originalYValues;
    vector<uint8_t> reconstructedYValues;
    double peakPSNR = 0;
    double avgPSNR = 0;
    while(true){
        // Frame returned is a Mat with only the Y channel
        Mat originalFrame = originalMovie.read_frame(originalFile);
        Mat reconstructedFrame = reconstructedMovie.read_frame(reconstructedFile);

        if (originalFrame.empty() || reconstructedFrame.empty()){
            break;
        }

        // Get the Y values from the original frame
        for (int i = 0; i < originalFrame.rows; i++){
            for (int j = 0; j < originalFrame.cols; j++){
                originalYValues.push_back(originalFrame.at<uint8_t>(i, j));
            }
        }

        // Get the Y values from the reconstructed frame
        for (int i = 0; i < reconstructedFrame.rows; i++){
            for (int j = 0; j < reconstructedFrame.cols; j++){
                reconstructedYValues.push_back(reconstructedFrame.at<uint8_t>(i, j));
            }
        }

        // Calculate PSNR
        double e2 = 0; // Mean squared error
        for (int i = 0; i < originalYValues.size(); i++){
            e2 += pow((originalYValues[i] - reconstructedYValues[i]), 2);
        }
        e2 /= originalYValues.size();

        double psnr = 10 * log10(pow(255, 2) / e2);

        if (psnr != INFINITY){
            // Update peak PSNR
            if (psnr > peakPSNR){
                peakPSNR = psnr;
            }

            // Update average PSNR
            avgPSNR += psnr;
        }

        // Clear the vectors
        originalYValues.clear();
        reconstructedYValues.clear();
    }

    std::cout << "Peak PSNR: " << peakPSNR << std::endl;

    // Calculate average PSNR
    avgPSNR /= originalMovie.get_number_frames();
    std::cout << "Average PSNR: " << avgPSNR << std::endl;

    return 0;
}