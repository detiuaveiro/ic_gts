#include "movie.h"

bool Movie::check_contains_frame(string line) {
    // check if last 5 characters are "FRAME"
    return line.length() >= 5 && (line.substr(line.length() - 5) == "FRAME");
}

// This function is validated to end on the correct byte
HeaderParameters Movie::get_header_parameters(std::fstream& stream) {
    if (!stream.is_open()) {
        cerr << "Error: movie file is not open" << std::endl;
    }

    // check appropriate format

    stream.seekg(0, std::ios::end);
    headerParameters.fileSize = stream.tellg();
    stream.seekg(0, std::ios::beg);

    char ch;
    string header = "";
    // Attention that '\n' after Frame get's deleted
    while (stream.get(ch) && !check_contains_frame(header)) {
        header += ch;
    }

    //cout << "Header: " << header << endl;

    // Takes only space separated C++ strings.
    stringstream headerStream(header);
    string parameter;
    // Extract word from the stream (remove whitespaces)
    int count = 0;
    while (headerStream >> parameter) {
        if (count == 0)
            headerParameters.format = parameter;
        else if (count == 1)
            headerParameters.chroma = parameter;
        else if (count == 2)
            headerParameters.width = std::stoi(parameter.substr(1));  // Skip 'W'
        else if (count == 3)
            headerParameters.height = std::stoi(parameter.substr(1));  // Skip 'H'
        else if (count == 4) {
            size_t pos = parameter.find(':');
            if (pos != std::string::npos) {
                int frameRateNumerator = std::stoi(parameter.substr(1, pos - 1));  // before ':'
                int frameRateDenominator = std::stoi(parameter.substr(pos + 1));   // after ':'
                headerParameters.fps = static_cast<int>(frameRateNumerator) / frameRateDenominator;
            }
        } else if (count == 5)
            headerParameters.interlace = parameter;
        else if (count == 6)
            headerParameters.aspectRatio = parameter;

        count++;
    }

    // Attention, only tested for C420jpeg
    /*Frame Size = W * H + 1.5 * [(W / 2) * (H / 2) + (W / 2) * (H / 2)]*/
    headerParameters.frameSize = headerParameters.width * headerParameters.height * 1.5;

    // Count header, but remove the inclusion of Frame keyword
    int headerLength = header.length() - 6;

    // Excluding header size
    int64_t frameFileSize = headerParameters.fileSize - headerLength;

    // Include Frame keyword
    int frameS = headerParameters.frameSize + 6;
    headerParameters.numberFrames = static_cast<int>(frameFileSize / frameS);

    return headerParameters;
}

Mat Movie::read_frame(std::fstream& stream) {
    if (!stream.is_open()) {
        std::cerr << "Error: movie file is not open" << std::endl;
        return cv::Mat();
    }

    // Check for the end of file
    if (stream.eof()) {
        std::cerr << "Reached end of file while reading frame" << std::endl;
        return cv::Mat();  // Return an empty Mat object or handle appropriately
    }

    // Allocate memory to read a frame, but only Y plane
    int dataToRead = (int)((double)headerParameters.frameSize / 1.5);
    char* frameData = new char[dataToRead + 1];
    frameData[dataToRead] = '\0';  // add termination character

    // Read the frame data
    stream.read(frameData, dataToRead);

    std::ofstream file("./temp_file.txt");  // Open the file for writing

    if (file.is_open()) {
        file << frameData;  // Write the char* data to the file
        file.close();       // Close the file stream
        std::cout << "Data successfully written to file" << std::endl;
    } else {
        std::cerr << "Unable to open file" << std::endl;
    }

    // Ignore the remaining planes and "FRAME\n" tag at the beginning of the next frame
    int offset = 20;
    stream.ignore(headerParameters.frameSize - dataToRead + offset, '\n');

    // Convert the frame data to a Mat object with just the Y field and make a deep copy
    cv::Mat frame =
        cv::Mat(headerParameters.height, headerParameters.width, CV_8UC1, frameData).clone();

    delete[] frameData;  // Free memory

    return frame;
}
