#ifndef WAVEFFECTS_H
#define WAVEFFECTS_H

#include <cstring>
#include <iostream>
#include <map>
#include <sndfile.hh>
#include <string>
#include <vector>

#define SAMPLE_RATE 44100     // Assuming ONLY!
#define MAX_BUFFER_SIZE 8192  // Max buffer size for processing audio data
constexpr size_t FRAMES_BUFFER_SIZE = 65536;  // Buffer for reading frames

using namespace std;

enum Effects {
    NONE,
    ECHOE,
    AMPLITUDE_MODULATION,
    TIME_VARYING_DELAYS,
    DELAY,
    REVERSE,
    SPEED_UP,
    SLOW_DOWN,
    CHORUS,
    INVERT,
    MONO
};

namespace EffectsInfo {
string outputFileName = "output.wav";
string inputFileName = "sample.wav";
Effects effect = NONE;
vector<double> param;
bool effectChosen = false;
};  // namespace EffectsInfo

void setEffect(Effects effect, const char* arg);

class WAVEffects {
   private:
    Effects effect;
    int sampleRate;
    vector<double> arg;

    static double feedback_lines(const std::vector<double>& inputSamples,
                                 std::vector<double>& outputSamples,
                                 uint8_t numLines, double decay,
                                 int sampleDelay, double sample) {
        for (int i = 0; i < numLines; i++) {
            // Single echoe: y[n] = x[n] + decay * y[n - Delay]
            // Multiple echoe: y[n] = x[n] + decay1 * y[n - D1] + decay2 * y[n - D2]...
            // Maybe apply reduction value, to get D1,D2,decay1,decay2
            sample += decay * inputSamples[i - sampleDelay];
        }

        return sample;
    }

   public:
    WAVEffects(Effects eff, vector<double> param, int sampleR) {
        effect = eff;
        arg = param;
        sampleRate = sampleR;
    }

    void effect_echo(const std::vector<double>& inputSamples,
                     std::vector<double>& outputSamples) {
        // arguments
        if (arg.size() != 3)
            throw std::invalid_argument(
                "Expected 3 arguments for echo effect (number of echoes, delay "
                "and gain/decay)");

        uint8_t nLines = static_cast<uint8_t>(arg[0]);
        if (nLines == 0)
            throw std::invalid_argument(
                "Invalid number of lines (needs to be > 0)");
        double delay = arg[1];
        if (delay == 0)
            throw std::invalid_argument("Invalid delay (needs to be > 0)");
        double decay = arg[2];
        if (decay == 0)
            throw std::invalid_argument(
                "Invalid gain/decay (needs to be between 0 < x < 1)");

        int delaySamples = static_cast<int>(
            delay * this->sampleRate);  // 1 * 44100, 2 * 44100...

        for (long i = 0; i < (long)inputSamples.size(); i++) {
            double echoSample = inputSamples[i];  // x[n]
            if (i >= delaySamples) {
                echoSample = feedback_lines(inputSamples, outputSamples, nLines,
                                            decay, delaySamples, echoSample);
                echoSample /= (1 + decay);
            }
            outputSamples.push_back(echoSample);
        }
    }

    void effect_amplitude_modulation(const std::vector<double>& input,
                                     std::vector<double>& output,
                                     double modulationFrequency) {}

    void effect_time_varying_delays(const std::vector<double>& input,
                                    std::vector<double>& output,
                                    double delaySeconds) {}

    void effect_delay(const std::vector<double>& input,
                      std::vector<double>& output, double delaySeconds) {}

    void effect_reverse(const std::vector<double>& input,
                        std::vector<double>& output) {}

    void effect_speed_up(const std::vector<double>& input,
                         std::vector<double>& output, double factor) {}

    void effect_slow_down(const std::vector<double>& input,
                          std::vector<double>& output, double factor) {}

    void effect_chorus(const std::vector<double>& input,
                       std::vector<double>& output, double delaySeconds,
                       double decay) {}

    void effect_invert(const std::vector<double>& input,
                       std::vector<double>& output) {}

    void effect_mono(const std::vector<double>& input,
                     std::vector<double>& output) {}
};
#endif