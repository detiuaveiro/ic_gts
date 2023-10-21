#include <iostream>
#include <vector>
#include <sndfile.hh>
#include <cmath>

using namespace std;

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		cerr << "Usage: " << argv[0] << " <input file> <input2 file>\n";
		return 1;
	}

	SndfileHandle sndFile1{argv[argc - 2]};
	if (sndFile1.error())
	{
		cerr << "Error: invalid first input file\n";
		return 1;
	}

	if ((sndFile1.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV)
	{
		cerr << "Error: first file is not in WAV format\n";
		return 1;
	}

	if ((sndFile1.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16)
	{
		cerr << "Error: first file is not in PCM_16 format\n";
		return 1;
	}

	SndfileHandle sndFile2{argv[argc - 1]};
	if (sndFile2.error())
	{
		cerr << "Error: invalid second input file\n";
		return 1;
	}

	if ((sndFile2.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV)
	{
		cerr << "Error: second file is not in WAV format\n";
		return 1;
	}

	if ((sndFile2.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16)
	{
		cerr << "Error: second file is not in PCM_16 format\n";
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
    vector<double> channelMaxErrors(numChannels); // Maximum per sample absolute error
    vector<double> SNRs(numChannels);

    for(int channel = 0; channel < numChannels; ++channel)
    {
        // Norma L² = sqrt(x1² + x2² + ... + xN²)/n
        // Sendo x1 a xN as diferenças entre a data do ficheiro posterior e do original
        // n -> Nº frames gerados
        vector<double> audio1_samples(sndFile1.frames());
        vector<double> audio2_samples(sndFile2.frames());

        sndFile1.read(audio1_samples.data(), sndFile1.frames());
        sndFile2.read(audio2_samples.data(), sndFile2.frames());
    
        double channelError = 0.0; //Or energy Noise
        double channelMaxError = 0.0;
        double energySignal = 0.0;

        for (long unsigned int i = 0; i < audio1_samples.size(); ++i)
        {
            double diff = audio1_samples[i] - audio2_samples[i];
            channelError += pow(diff,2);
            // L∞ norm
            if(diff > channelMaxError){
                channelMaxError = diff;
            }
            energySignal += pow(audio1_samples[i],2);
        }
        
        channelErrors[channel] = sqrt(channelError / sndFile1.frames());
        channelMaxErrors[channel] = channelMaxError;
        SNRs[channel] = 10*log10(energySignal/channelError); // 10*log10(energySignal/energyNoise) ou 10*log10(potenciaSignal/potenciaNoise)
    }

    // Calculate the average L2 norm (mean squared error) across all channels
    double averageError = 0.0;
    double averageMaxError = 0.0;
    double averageSNR = 0.0;
    for (int channel = 0; channel < numChannels; ++channel)
    {
        averageError += channelErrors[channel];
        averageMaxError += channelMaxErrors[channel];
        averageSNR += SNRs[channel];
    }
    averageError /= numChannels;
    averageMaxError /= numChannels;
    averageSNR /= numChannels;

    for(int channel = 0; channel < numChannels; ++channel)
    {
        cout << "Channel " << channel + 1 << " MSE: " << channelErrors[channel] << endl;
        cout << "Channel " << channel + 1 << " Max sample error: " << channelMaxErrors[channel] << endl;
        cout << "Channel " << channel + 1 << " SNR: " << SNRs[channel] << endl;
    }

    cout << "Average MSE of the channels: " << averageError << endl;
    cout << "Average Max sample error fo the channels: " << averageMaxError << endl;
    cout << "Average SNR of the channels: " << averageSNR << endl;

    return 0;
}