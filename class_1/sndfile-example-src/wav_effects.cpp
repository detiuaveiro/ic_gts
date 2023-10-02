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

    SndfileHandle sfhIn{argv[argc - 1]};
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


    return 0;
}