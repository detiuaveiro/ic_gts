#include "wav_effects.h"

using namespace std;

static void print_usage() {
    cerr
        << "Usage: %s [OPTIONS] [EFFECTS] <inputFile>\n"
           "  OPTIONS:\n"
           "  -h, --help        --- print this help\n"
           "  -o, --output      --- set output file name (default: "
           "output.wav)\n"
        << "  EFFECTS:\n"
           "  -e n d g          --- apply echo effect, [number_echoes], "
           "[delay], [gain] (suggested: 1, 1.0, 0.5)\n"
           "  -a f              --- apply amplitude modulation effect, f is "
           "frequency (default: 1)\n"
           "  -d time           --- apply delay, time in seconds (default: 1)\n"
           "  -f time           --- advance in music, time in seconds "
           "(default: 1)\n"
           "  -r                --- apply reverse effect\n"
           "  -s x              --- apply speed up effect (default: 10%)\n"  // attention, prob remove
           "  -b x              --- apply slow down effect (default: 10%)\n"  // attention, prob remove
           "  -i                --- apply invert effect\n"
           "  -m                --- apply mono effect (convert all channels to "
           "one)\n"
           "  -l                --- play only left channel\n"
           "  -p                --- play only right channel\n"
        << endl;
}

void setEffect(Effects effect, char* val, double arg) {
    EffectsInfo::effect = effect;
    if (val == nullptr)
        return;
    if (!isdigit(*val)) {
        cerr << "Error: Expecting numerical value, but received " << val
             << " instead" << endl;
        exit(1);
    }
    try {
        EffectsInfo::param.push_back(arg);
    } catch (std::invalid_argument& e) {
        std::cerr << "Error: Invalid argument for option." << std::endl;
        exit(1);
    }
}

int check_wav_file(SndfileHandle& musicFile) {
    if (musicFile.error()) {
        cerr << "Error: invalid input file\n";
        return -1;
    }

    if ((musicFile.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV) {
        cerr << "Error: file is not in WAV format\n";
        return -1;
    }

    if ((musicFile.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16) {
        cerr << "Error: file is not in PCM_16 format\n";
        return -1;
    }

    return 0;
}

int process_arguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {  // Start for at 1, to skip program name
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage();
            return 1;
        } else if (strcmp(argv[i], "-o") == 0 ||
                   strcmp(argv[i], "--output") == 0) {
            i++;  // Move to the next argument for the output file name
            if (i < (argc - 1)) {
                EffectsInfo::outputFileName = argv[i];
            } else {
                std::cerr << "Error: Missing argument for -o/--output option."
                          << std::endl;
                return -1;
            }
        } else if (!EffectsInfo::effectChosen && strcmp(argv[i], "-e") == 0) {
            i++;
            if (i < (argc - 3)) {
                setEffect(ECHOE, argv[i], atof(argv[i]));
                setEffect(ECHOE, argv[i + 1], atof(argv[i + 1]));
                setEffect(ECHOE, argv[i + 2], atof(argv[i + 2]));
            } else {
                std::cerr << "Error: Missing argument for -e option."
                          << std::endl;
                return -1;
            }
            EffectsInfo::effectChosen = true;
        } else if (!EffectsInfo::effectChosen && strcmp(argv[i], "-a") == 0) {
            i++;
            if (i < (argc - 1)) {
                setEffect(AMPLITUDE_MODULATION, argv[i], atof(argv[i]));
            } else {
                char def = '1';
                setEffect(AMPLITUDE_MODULATION, &def, 1.0);
            }
            EffectsInfo::effectChosen = true;
        } else if (!EffectsInfo::effectChosen && strcmp(argv[i], "-d") == 0) {
            i++;
            if (i < (argc - 1)) {
                setEffect(DELAY, argv[i], atof(argv[i]));
            } else {
                char def = '1';
                setEffect(DELAY, &def, 1.0);
            }
            EffectsInfo::effectChosen = true;
        } else if (!EffectsInfo::effectChosen && strcmp(argv[i], "-f") == 0) {
            i++;
            if (i < (argc - 1)) {
                setEffect(FORWARD, argv[i], atof(argv[i]));
            } else {
                char def = '1';
                setEffect(FORWARD, &def, 1.0);
            }
            EffectsInfo::effectChosen = true;
        } else if (!EffectsInfo::effectChosen && strcmp(argv[i], "-r") == 0) {
            setEffect(REVERSE, nullptr, INT32_MAX);
            EffectsInfo::effectChosen = true;
        } else if (!EffectsInfo::effectChosen && strcmp(argv[i], "-s") == 0) {
            i++;
            if (i < (argc - 1)) {
                setEffect(SPEED_UP, argv[i], atof(argv[i]));
            } else {
                char def = '1';
                setEffect(SPEED_UP, &def, 10.0);
            }
            EffectsInfo::effectChosen = true;
        } else if (!EffectsInfo::effectChosen && strcmp(argv[i], "-b") == 0) {
            i++;
            if (i < (argc - 1)) {
                setEffect(SLOW_DOWN, argv[i], atof(argv[i]));
            } else {
                char def = '1';
                setEffect(SLOW_DOWN, &def, 10.0);
            }
            EffectsInfo::effectChosen = true;
        } else if (!EffectsInfo::effectChosen && strcmp(argv[i], "-i") == 0) {
            setEffect(INVERT, nullptr, INT32_MAX);
            EffectsInfo::effectChosen = true;
        } else if (!EffectsInfo::effectChosen && strcmp(argv[i], "-m") == 0) {
            setEffect(MONO, nullptr, INT32_MAX);
            EffectsInfo::effectChosen = true;
        } else if (!EffectsInfo::effectChosen && strcmp(argv[i], "-l") == 0) {
            setEffect(LEFT_CHANNEL_ONLY, nullptr, INT32_MAX);
            EffectsInfo::effectChosen = true;
        } else if (!EffectsInfo::effectChosen && strcmp(argv[i], "-p") == 0) {
            setEffect(RIGHT_CHANNEL_ONLY, nullptr, INT32_MAX);
            EffectsInfo::effectChosen = true;
            // checks if the user introduced something unknown that starts with a '-'
        } else if (argv[i][0] == '-') {
            std::cerr << "Error: Unknown option or argument: " << argv[i]
                      << std::endl;
            return -1;
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " [EFFECT] <inputFile>\n";
        return 1;
    }
    int ret = process_arguments(argc, argv);
    if (ret < 0)
        return 1;
    else if (ret > 0)
        return 0;

    /* Check .wav files */
    EffectsInfo::inputFileName = argv[argc - 1];
    SndfileHandle sfhIn{EffectsInfo::inputFileName};
    if (check_wav_file(sfhIn) < 0)
        return 1;

    int channels = sfhIn.channels();
    if (EffectsInfo::effect == MONO)
        channels = 1;

    SndfileHandle sfhOut{EffectsInfo::outputFileName, SFM_WRITE, sfhIn.format(),
                         channels, sfhIn.samplerate()};

    if (sfhOut.error()) {
        cerr << "Error: problem encountered generating file "
             << EffectsInfo::outputFileName << endl;
        return 1;
    }

    // Create a buffer for reading and writing audio data
    std::vector<short> inputSamples(FRAMES_BUFFER_SIZE * sfhIn.channels());
    std::vector<short> outputSamples;
    std::vector<short> reverseBuffer;
    size_t nFrames;

    // Effects class
    WAVEffects effects{EffectsInfo::effect, EffectsInfo::param,
                       sfhIn.samplerate()};

    // Read and process audio data in chunks
    while ((nFrames = sfhIn.readf(inputSamples.data(), FRAMES_BUFFER_SIZE))) {

        inputSamples.resize(nFrames * sfhIn.channels());
        // Apply the desired effect to the input buffer
        // Using switch because it's faster ehehe: https://www.youtube.com/watch?v=fjUG_y5ZaL4&pp=ygUNc3dpdGNoIGZhc3Rlcg%3D%3D
        switch (EffectsInfo::effect) {
            case ECHOE:
                effects.effect_echo(inputSamples, outputSamples);
                break;
            case AMPLITUDE_MODULATION:
                effects.effect_amplitude_modulation(inputSamples,
                                                    outputSamples);
                break;
            case DELAY:
                effects.effect_delay(inputSamples, outputSamples);
                break;
            case FORWARD:
                effects.effect_forward(inputSamples, outputSamples);
                break;
            case SPEED_UP:
                effects.effect_speed_up(inputSamples, outputSamples);
                break;
            case SLOW_DOWN:
                effects.effect_slow_down(inputSamples, outputSamples);
                break;
            case INVERT:
                effects.effect_invert(inputSamples, outputSamples);
                break;
            case REVERSE:
                reverseBuffer.insert(reverseBuffer.end(), inputSamples.begin(),
                                     inputSamples.end());
                break;
            case MONO:
                effects.effect_mono(inputSamples, outputSamples);
                break;
            case LEFT_CHANNEL_ONLY:
                effects.effect_merge_left_channel(inputSamples, outputSamples);
                break;
            case RIGHT_CHANNEL_ONLY:
                effects.effect_merge_right_channel(inputSamples, outputSamples);
                break;
            default:
                cerr << "The specified effect is not supported.\n";
                return 1;
        };
    }

    if (EffectsInfo::effect == REVERSE)
        effects.effect_reverse(reverseBuffer, outputSamples);

    // Write the modified audio data to the output file
    //  (divide by the number of channels, since they all get mixed up)
    if (EffectsInfo::effect == MONO)
        sfhOut.writef(outputSamples.data(), outputSamples.size());
    else
        sfhOut.writef(outputSamples.data(),
                      outputSamples.size() / sfhIn.channels());

    std::cout << "Effect applied and saved to " << EffectsInfo::outputFileName
              << std::endl;

    return 0;
}