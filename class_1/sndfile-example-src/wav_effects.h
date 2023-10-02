#ifndef WAVEFFECTS_H
#define WAVEFFECTS_H

#include <iostream>
#include <cstring>
#include <string>
#include <sndfile.hh>

using namespace std;

enum Effects{
    NONE,
    ECHOE,
    AMPLITUDE_MODULATION,
    TIME_VARYING_DELAYS,
    DELAY,
    REVERSE,
    SPEED_UP,
    SLOW_DOWN,
    CHORUS,
    INVERT
};

namespace EffectsInfo{
    string outputFileName = "output.wav";
    Effects effect = NONE;
    char param;
};

class WAVEffects
{
private:
    

public:
    WAVEffects(){
        
    }



};
#endif