#include "movie.h"

bool Movie::check_contains_frame(string line) {
    // check if last 5 characters are "FRAME"
    return line.length() >= 5 && (line.substr(line.length() - 5) == "FRAME");
}

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

    // Calculate frame size based on chroma subsampling
    int chroma_subsampling_horizontal_factor = 1;
    int chroma_subsampling_vertical_factor = 1;

    if ((headerParameters.chroma).compare("C420jpeg") == 0) {
        chroma_subsampling_horizontal_factor = 2;
        chroma_subsampling_vertical_factor = 2;
    } else if ((headerParameters.chroma).compare("C422jpeg") == 0) {
        chroma_subsampling_horizontal_factor = 2;
        chroma_subsampling_vertical_factor = 1;
    } else if ((headerParameters.chroma).compare("C444") == 0) {
        chroma_subsampling_horizontal_factor = 1;
        chroma_subsampling_vertical_factor = 1;
    }

    headerParameters.frameSize =
        headerParameters.width * headerParameters.height *
        (1 + 2 * (chroma_subsampling_horizontal_factor / chroma_subsampling_vertical_factor));

    // Count header, but remove the inclusion of Frame keyword
    int headerLength = header.length() - 5;

    // Excluding header size
    int64_t frameFileSize = headerParameters.fileSize - headerLength;

    // Include Frame keyword
    int frameS = headerParameters.frameSize + 5;
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
    int dataToRead = headerParameters.frameSize / 3;
    char* frameData = new char[dataToRead];

    // Read the frame data
    stream.read(frameData, dataToRead);
    int nCounter = 0;

    for (int i = 0; i < dataToRead; ++i) {
        // Check for newline character
        if (frameData[i] == '\n') {
            nCounter++;
        }

        // Check for "FRAME"
        if (i + 4 <= dataToRead && std::strncmp(frameData + i, "FRAME", 5) == 0) {
            std::cerr << "The frameData contains 'FRAME' at " << i << " / " << dataToRead << ".\n";
            exit(2);
        }
    }

    if (nCounter > 1) {
        //std::cerr << "The frameData contains a newline character '\\n' at " << i << " / "
        //          << dataToRead << ".\n";
        std::cerr << "The frameData contains "<< nCounter << " newline characters '\\n'" << endl;
        exit(1);
    }

    // Ignore the remaining planes
    stream.ignore(headerParameters.frameSize - dataToRead);

    // Convert the frame data to a Mat object with just the Y field
    cv::Mat frame(headerParameters.height, headerParameters.width, CV_8UC1, frameData);

    // Ignore the "FRAME\n" tag at the beginning of the next frame data
    std::string frameTag = "\nFRAME\n";
    stream.ignore(frameTag.length());

    delete[] frameData;  // Free memory

    return frame;
}
