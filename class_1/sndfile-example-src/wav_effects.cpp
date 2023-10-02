#include "wav_effects.h"

using namespace std;

static void print_usage(){
    cerr << "Usage: %s [OPTIONS] [EFFECTS] <inputFile>\n"
           "  OPTIONS:\n"
           "  -h, --help        --- print this help\n"
           "  -o, --output      --- set output file name (default: output.wav)\n"
        << "  EFFECTS:\n"
           "  -e echoes         --- apply echo effect, from single=1 to multiple=n echoes  (default: 1)\n"
           "  -a                --- apply amplitude modulation effect\n"
           "  -t                --- apply Time-varying delays\n"
           "  -d time           --- apply delay, time in seconds (default: 1)\n"
           "  -r                --- apply reverse effect\n"
           "  -s \%             --- apply speed up effect (default: 10%)\n"
           "  -d \%             --- apply slow down effect (default: 10%)\n"
           "  -c                --- apply chorus effect\n"
           "  -i                --- apply invert effect\n"
        << endl;
}

void setEffect(Effects effect, const char* arg) {
    EffectsInfo::effect = effect;
    if(arg == nullptr) 
        return;
    try {
        EffectsInfo::param = *arg;
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: Invalid argument for option." << std::endl;
        exit(1);
    }
}

void WAVEffects::effect_echo(const std::vector<double>& input, std::vector<double>& output, double delaySeconds, double decay) {
    int delaySamples = static_cast<int>(delaySeconds * sampleRate); 

    for (long i = 0; i < (long) input.size(); i++) {
        double echoSample = input[i];
        if (i >= delaySamples) {
            echoSample += decay * input[i - delaySamples];
        }
        output.push_back(echoSample);
    }
}

int main(int argc, char *argv[])
{   
    // Parse command line arguments
    if (argc < 2)
	{
		cerr << "Usage: " << argv[0] << " [EFFECT] <inputFile>\n";
		return 1;
	}
    for (int i = 1; i < argc; i++) { // Start for at 1, to skip program name
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage();
            return 0;
        } else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            i++; // Move to the next argument for the output file name
            if (i < (argc-1)) {
                EffectsInfo::outputFileName = argv[i];
            } else {
                std::cerr << "Error: Missing argument for -o/--output option." << std::endl;
                return 1;
            }
        } else if (strcmp(argv[i], "-e") == 0) {
            i++;
            if (i < (argc-1)) {
                setEffect(ECHOE, argv[i]);
            } else {
                std::cerr << "Error: Missing argument for -e option." << std::endl;
                return 1;
            }
        } else if (strcmp(argv[i], "-a") == 0) {
            setEffect(AMPLITUDE_MODULATION, nullptr);
        } else if (strcmp(argv[i], "-t") == 0) {
            setEffect(TIME_VARYING_DELAYS, nullptr);
        } else if (strcmp(argv[i], "-d") == 0) {
            i++;
            if (i < (argc-1)) {
                setEffect(DELAY, argv[i]);
            } else {
                std::cerr << "Error: Missing argument for -d option." << std::endl;
                return 1;
            }
        } else if (strcmp(argv[i], "-r") == 0) {
            setEffect(REVERSE, nullptr);
        } else if (strcmp(argv[i], "-s") == 0) {
            setEffect(SPEED_UP, nullptr);
        } else if (strcmp(argv[i], "-b") == 0) {
            setEffect(SLOW_DOWN, nullptr);
        } else if (strcmp(argv[i], "-c") == 0) {
            setEffect(CHORUS, nullptr);
        } else if (strcmp(argv[i], "-i") == 0) {
            setEffect(INVERT, nullptr);
        // checks if the user introduced something unknown that starts with a '-'
        } else if (argv[i][0] == '-') {
            std::cerr << "Error: Unknown option or argument: " << argv[i] << std::endl;
            return 1;
        }
        
    }

    EffectsInfo::inputFileName = argv[argc - 1];

    SndfileHandle sfhIn{EffectsInfo::inputFileName};
    if (sfhIn.error())
    {
        cerr << "Error: invalid input file\n";
        return 1;
    }

	if ((sfhIn.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV)
	{
		cerr << "Error: file is not in WAV format\n";
		return 1;
	}

	if ((sfhIn.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16)
	{
		cerr << "Error: file is not in PCM_16 format\n";
		return 1;
	}

    // Open the input audio file
    SF_INFO inputInfo;
    SNDFILE* inputFileHandle = sf_open(EffectsInfo::inputFileName.c_str(), SFM_READ, &inputInfo);
    if (!inputFileHandle) {
        std::cerr << "Error: Failed to open input file." << std::endl;
        return 1;
    }

    // Initialize output audio file parameters
    SF_INFO outputInfo;
    outputInfo.format = inputInfo.format;
    outputInfo.channels = inputInfo.channels;
    outputInfo.samplerate = inputInfo.samplerate;

    // Create a buffer for reading and writing audio data
    std::vector<double> inputBuffer(MAX_BUFFER_SIZE);
    std::vector<double> outputBuffer;

    // Create the output audio file
    SNDFILE* outputFileHandle = sf_open(EffectsInfo::outputFileName.c_str(), SFM_WRITE, &outputInfo);
    if (!outputFileHandle) {
        std::cerr << "Error: Failed to create output file." << std::endl;
        sf_close(inputFileHandle);
        return 1;
    }

    // Effects class
    WAVEffects effects{EffectsInfo::effect, EffectsInfo::param};

    // Read and process audio data in chunks
    while (true) {
        sf_count_t bytesRead = sf_readf_double(inputFileHandle, inputBuffer.data(), MAX_BUFFER_SIZE);
        if (bytesRead <= 0) {
            break; // EOF
        }

        double delaySeconds = std::stod(argv[3]);
        double decay = 0.5; // Adjust this for echo decay strength

        // Apply the desired effect to the input buffer
        if(EffectsInfo::effect == ECHOE)
            effects.effect_echo(inputBuffer, outputBuffer, delaySeconds, decay);

        // Write the modified audio data to the output file
        sf_writef_double(outputFileHandle, outputBuffer.data(), bytesRead);
        outputBuffer.clear();
    }

    // Close the input and output files
    sf_close(inputFileHandle);
    sf_close(outputFileHandle);

    std::cout << "Echo effect applied and saved to " << EffectsInfo::outputFileName << std::endl;

    return 0;
}