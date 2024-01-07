#include "movie.h"

void Movie::getHeaderParameters(std::fstream& movie) {
    if (!movie.is_open()) {
        cerr << "Error: movie file is not open" << std::endl;
    }

    movie.seekg(0, std::ios::end);
    int64_t fileSize = movie.tellg();
    movie.seekg(0, std::ios::beg);

    string line;
    getline(movie, line);
    for (size_t i = 9; i < line.length(); i++) {
        switch (line[i]) {
            case 'W':
                headerParameters.width = stoi(getParameter(line, i, 'W'));
                break;

            case 'H':
                headerParameters.height = stoi(getParameter(line, i, 'H'));
                break;

            case 'C':
                headerParameters.chroma = stoi(getParameter(line, i, 'C'));

                break;
            case 'F':
                headerParameters.fps = getParameter(line, i, 'F');

                break;
            default:
                break;
        }
    }

    //For 4:2:0 chroma
    //IMPORTANT: THIS IS THE SIZE OF THE Y CHANNEL ONLY,
    //It would be times 3/2 if it was all
    headerParameters.bytesPerFrame =
        headerParameters.height * headerParameters.width;

    int frameDataSize =
        headerParameters.bytesPerFrame * 3 / 2;  // Assuming 4:2:0 format

    if (fileSize > 0 && frameDataSize > 0) {
        int64_t totalFrameDataSize =
            fileSize - line.length();  // Excluding header size
        headerParameters.numberFrames =
            static_cast<int>(totalFrameDataSize / frameDataSize);
    } else {
        headerParameters.numberFrames = 0;  // Unable to determine frame count
    }
}

string Movie::getParameter(string line, size_t startPos, char parameterType) {
    size_t endPos = line.find_first_of(' ', startPos);

    string parameter;

    //4:2:0 Chroma parameter is expected to have jpeg after the actual parameter
    if (parameterType == 'C')
        parameter = line.substr(startPos + 1, endPos - startPos - 4);
    else {
        parameter = line.substr(startPos + 1, endPos - startPos - 1);
    }

    return parameter;
}

Mat Movie::readFrameFromMovie(std::fstream& movie) {
    if (!movie.is_open()) {
        cerr << "Error: movie file is not open" << std::endl;
        return Mat();
    }

    string line;

    //goodbit before the while is 0, but then changes
    //std::cout << "Goodbit before while: " << movie.good() << std::endl;

    //compare returns 0 if strings match
    //int i = 0;
    while (getline(movie, line) && line.compare("FRAME") != 0) {
        //Do nothing while waiting for a FRAME
        //So, iterates 1253 times, and then sets goodbit to 0, the loop is not stopping
        //std::cout << "Goodbit inside of while: " << movie.good() << std::endl;
        //i++;
        if (line.length() < 20) {
            std::cout << line << std::endl;
        }
    }

    //returns 1 in the first iteration (1 means no errors), on the remaining it returns 0,
    //means at least one of the error flags is set
    //std::cout << "Goodbit: " << movie.good() << std::endl;
    //std::cout << "iterates : " << i << std::endl;

    //std::cout << "movie peak: " << std::to_string(movie.peek()) << std::endl;
    if (movie.peek() == EOF) {
        //std::cout << "End of the y4m file" << std::endl;
        return Mat();
    }

    //std::cout << "current pos: " << movie.tellg() << std::endl;

    vector<uint8_t> frameData(headerParameters.bytesPerFrame);
    vector<uint8_t> garbage(headerParameters.bytesPerFrame / 2);
    //frameData.data() returns pointer to data inside vector
    //reinterpret_cast<char*> used for the * to be interpretated as a char* because readsome requires it to be
    movie.read(reinterpret_cast<char*>(frameData.data()),
               headerParameters.bytesPerFrame);
    movie.read(reinterpret_cast<char*>(garbage.data()),
               headerParameters.bytesPerFrame / 2);

    //std::cout << "Goodbit after read: " << movie.good() << std::endl;
    garbage.clear();

    std::cin.clear(movie.goodbit);
    std::cin.clear(movie.eofbit);
    std::cin.clear(movie.failbit);
    std::cin.clear(movie.badbit);

    //std::cout << "current pos after read: " << movie.tellg() << std::endl;

    return Mat(headerParameters.height, headerParameters.width, CV_8UC1,
               frameData.data());
}

/*
Mat Movie::readFrameFromMovie(std::fstream& movie) {
    if (!movie.is_open()) {
        std::cerr << "Error: movie file is not open" << std::endl;
        return cv::Mat();
    }

    std::string line;
    bool foundFrame = false;

    while (getline(movie, line)) {
        if (line.compare("FRAME") == 0) {
            foundFrame = true;
            break;
        }
    }

    if (!foundFrame) {
        std::cout << "End of the y4m file" << std::endl;
        return cv::Mat();
    }

    std::vector<uint8_t> frameData(headerParameters.bytesPerFrame);
    movie.read(reinterpret_cast<char*>(frameData.data()), headerParameters.bytesPerFrame);

    // Construct a CV_8UC1 (grayscale) image from Y channel data
    cv::Mat frame(headerParameters.height, headerParameters.width, CV_8UC1);
    for (int i = 0; i < headerParameters.height; ++i) {
        for (int j = 0; j < headerParameters.width; ++j) {
            frame.at<uint8_t>(i, j) = frameData[i * headerParameters.width + j];
        }
    }

    return frame;
}
*/