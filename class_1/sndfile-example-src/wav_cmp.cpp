#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <cmath>

using namespace std;

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		cerr << "Usage: " << argv[0] << " <audio file> <original file>\n";
		return 1;
	}

	SndfileHandle sndFile1{argv[argc - 2]};
	if (sndFile1.error())
	{
		cerr << "Error: invalid audio input file\n";
		return 1;
	}

	if ((sndFile1.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV)
	{
		cerr << "Error: file is not in WAV format\n";
		return 1;
	}

	if ((sndFile1.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16)
	{
		cerr << "Error: file is not in PCM_16 format\n";
		return 1;
	}

	SndfileHandle sndFile2{argv[argc - 1]};
	if (sndFile2.error())
	{
		cerr << "Error: invalid original input file\n";
		return 1;
	}

	if ((sndFile2.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV)
	{
		cerr << "Error: file is not in WAV format\n";
		return 1;
	}

	if ((sndFile2.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16)
	{
		cerr << "Error: file is not in PCM_16 format\n";
		return 1;
	}

    // Contagem de amostras tem de ser igual para serem comparadas
    if (sndFile1.frames() != sndFile2.frames())
    {
        cerr << "Error: audio and original files have different sample counts\n";
        return 1;
    }

    int numChannels = sndFile1.channels();
    vector<double> channelErrors(numChannels);

    for(int channel = 0; channel < numChannels; ++channel)
    {
        // Norma L² = sqrt(x1² + x2² + ... + xN²)/n
        // Sendo x1 a xN as diferenças entre a data do ficheiro posterior e do original
        // n -> Nº frames gerados
        vector<double> audioData1(sndFile1.frames());
        vector<double> audioData2(sndFile2.frames());

        sndFile1.read(audioData1.data(), sndFile1.frames());
        sndFile2.read(audioData2.data(), sndFile2.frames());
    
        double channelError = 0.0;

        for (int i = 0; i < sndFile1.frames(); ++i)
        {
            double diff = audioData1[i] - audioData2[i];
            channelError += pow(diff,2);
        }
        channelErrors[channel] = sqrt(channelError) / sndFile1.frames();
    }

    // Calculate the average L2 norm (mean squared error) across all channels
    double averageError = 0.0;
    for (int channel = 0; channel < numChannels; ++channel)
    {
        averageError += channelErrors[channel];
    }
    averageError /= numChannels;


    cout << "Mean squared error for each channel:\n";
    for(int channel = 0; channel < numChannels; ++channel)
    {
        cout << "Channel " << channel + 1 << ": " << channelErrors[channel] << endl;
    }

    return 0;
}