// Reduce the number of bits used to represent each audio sample (i.e., to perform uniform scalar quantization)

#include <iostream>
#include <vector>
#include <sndfile.hh>
#include "wav_quant.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

int main(int argc, char *argv[])
{

	if (argc < 4)
	{
		cerr << "Usage: " << argv[0] << " <input file> <quantization bits> <output file>\n";
		return 1;
	}

    SndfileHandle sfhIn{argv[argc - 3]};
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

    // Quantizacao -> 2^b niveis
    // Por ex:
    // 8 bits - 256 
    // 16 bits - 65536 (qualidade de audio CD)
    int quantBits{stoi(argv[argc - 2])};
    if(quantBits < 1 || quantBits > 15)
    {
        cerr << "Error: Number of quantization bits must be between 1 and 15\n";
		return 1;
    }

    SndfileHandle sfhOut { argv[argc-1], SFM_WRITE, sfhIn.format(),
	  sfhIn.channels(), sfhIn.samplerate() };
	if(sfhOut.error()) {
		cerr << "Error: invalid output file\n";
		return 1;
    }

    size_t nFrames;
    vector<short> samples(FRAMES_BUFFER_SIZE * sfhIn.channels());
    WAVQuant quant {quantBits};
    while((nFrames = sfhIn.readf(samples.data(), FRAMES_BUFFER_SIZE))) 
    {
        samples.resize(nFrames * sfhIn.channels());
        quant.quantize(samples);
        //quant.saveQuantizedSamples("quantized_samples.txt");
    }

    quant.toFile(sfhOut);
}