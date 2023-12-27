#include "movie.h"

void Movie::getHeaderParameters(std::fstream& movie) {
    if (!movie.is_open()) {
        cerr << "Error: movie file is not open" << std::endl;
    }

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
    headerParameters.bytesPerFrame = headerParameters.height * headerParameters.width;
}

string Movie::getParameter(string line, size_t startPos, char parameterType) {
    size_t endPos = line.find_first_of("\t", startPos);

    string parameter;

    //4:2:0 Chroma parameter is expected to have jpeg after the actual parameter
    if (parameterType == 'C')
        parameter = line.substr(startPos + 1, endPos - startPos - 4);
    else
        parameter = line.substr(startPos + 1, endPos - startPos);

    return parameter;
}

Mat Movie::readFrameFromMovie(std::fstream movie) {
    if (!movie.is_open()) {
        cerr << "Error: movie file is not open" << std::endl;
    }

    string line;

    vector<uint8_t> frameData;

    //frameData.data() returns pointer to data inside vector
    //reinterpret_cast<char*> used for the * to be interpretated as a char* because readsome requires it to be
    movie.read(reinterpret_cast<char*>(frameData.data()), headerParameters.bytesPerFrame);  

    return Mat(1, headerParameters.bytesPerFrame, CV_8UC1, frameData.data());
}