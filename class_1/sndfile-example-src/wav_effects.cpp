#include "wav_effects.h"

using namespace std;

static void print_usage(){
    cerr << "Usage: %s [OPTIONS] [EFFECTS] <inputFile>\n"
           "  OPTIONS:\n"
           "  -h, --help        --- print this help\n"
           "  -o, --output      --- set output file name (default: stdout)\n"
        << "  EFFECTS:\n"
           "  -e echos          --- apply echo effect, from single=1 to multiple=n echos  (default: 1)\n"
           "  -a                --- apply amplitude modulation effect\n"
           "  -t                --- apply Time-varying delays\n  "
           "  -d time           --- apply delay, time in seconds (default: 1)\n"
           "  -r                --- apply reverse effect\n"
           "  -s                --- apply speed up effect\n"
           "  -b                --- apply slow down effect\n"
           "  -c                --- apply chorus effect\n"
           "  -i                --- apply invert effect\n"
        << endl;
}

int main(int argc, char *argv[])
{
    for(int i = 0; i < argc; i++){
        if(strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
            print_usage();
        }
    }

	if (argc < 4)
	{
		cerr << "Usage: " << argv[0] << " [EFFECT] <inputFile>\n";
		return 1;
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