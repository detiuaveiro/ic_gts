#include "movie.h"

using namespace std;

void Movie::getHeaderParameters(std::fstream movie) {
    if (!movie.is_open()) {
        cerr << "Error: movie file is not open" << std::endl;
    }

    string line;
    getline(movie, line);

    for (size_t i = 9; i < line.length(); i++) {
        switch (line[i]) {
            case 'W':
                headerParameters.width = getParameter(line, i, 'W');
                break;

            case 'H':
                headerParameters.height = getParameter(line, i, 'H');
                break;

            case 'C':
                headerParameters.chroma = getParameter(line, i, 'C');
                break;
            case 'F':
                headerParameters.fps = getParameter(line, i, 'F');
                break;
            default:
                break;
        }
    }

    //for 4:2:0 chroma
    headerParameters.bytesPerFrame = headerParameters.height * headerParameters.width * (3/2);
}

uint16_t getParameter(string line, size_t startPos, char parameterType) {
    size_t endPos = line.find_first_of("\t", startPos);

    string parameter;

    //4:2:0 Chroma parameter is expected to have jpeg after the actual parameter
    if (parameterType == 'C')
        parameter = line.substr(startPos + 1, endPos - startPos - 4);
    else
        parameter = line.substr(startPos + 1, endPos - startPos);

    return stoi(parameter);
}

//LACKS ITERATION-LIKE WAY OF GETTING FRAMES
vector<uint8_t> Movie::readFrameFromMovie(std::fstream movie) {
    if (!movie.is_open()) {
        cerr << "Error: movie file is not open" << std::endl;
    }

    string line;

    vector<uint8_t> frameData;

    //frameData.data() returns pointer to data inside vector
    //reinterpret_cast<char*> used for the * to be interpretated as a char* because readsome requires it to be
    movie.readsome(reinterpret_cast<char*>(frameData.data()), headerParameters.bytesPerFrame);

    return frameData;
}