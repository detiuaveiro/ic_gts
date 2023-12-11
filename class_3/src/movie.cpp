#include "movie.h"

using namespace std;

void Movie::getHeaderParameters(std::fstream movie){
    if(!movie.is_open()){
        cerr << "Error: movie file is not open" << std::endl;
    }

    string line;
    getline(movie, line);

    for (size_t i = 9; i < line.length(); i++)
    {
        switch (line[i])
        {
        case 'W':
            headerParameters.push_back(getParameter(line, i));
            break;
        
        case 'H':
            headerParameters.push_back(getParameter(line, i));
            break;

        case 'F':
            headerParameters.push_back(getParameter(line, i));
            break;
        default:
            break;
        }
    }
}
    
uint16_t getParameter(string line, size_t startPos){
    size_t endPos = line.find_first_of("\t", startPos);
    
    string parameter = line.substr(startPos+1, endPos - startPos);

    return stoi(parameter);
}

vector<uint8_t> Movie::readFrameFromMovie(std::fstream movie){
    if(!movie.is_open()){
        cerr << "Error: movie file is not open" << std::endl;
    }

    string line;

    while (getline(movie, line))
    {
        
    }
    
}