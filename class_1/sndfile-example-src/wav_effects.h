#ifndef WAVEFFECTS_H
#define WAVEFFECTS_H

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <sndfile.hh>

#define SAMPLE_RATE 44100 // Assuming ONLY!
#define MAX_BUFFER_SIZE 8192 // Max buffer size for processing audio data

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
    string inputFileName = "sample.wav";
    Effects effect = NONE;
    char param;
};

void setEffect(Effects effect, const char* arg);

class WAVEffects
{
private:
    Effects effect;
    int sampleRate = SAMPLE_RATE;
    char arg;

public:
    WAVEffects(Effects eff, char param){
        effect = eff;
        arg = param;
    }

    void effect_echo(const std::vector<double>& input, std::vector<double>& output, double delaySeconds, double decay);


};
#endif