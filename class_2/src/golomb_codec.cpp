#include "golomb_codec.h"

int predict1(int a, int b, int c) {
    return 3 * a - 3 * b + c;
};

/*
##############################################################################
###################         Golomb Encoder Code            ###################
##############################################################################
*/

GEncoder::GEncoder(std::string outFileName, int m = -1,
                   PREDICTOR pred = AUTOMATIC)
    : writer('w', outFileName), golomb(DEFAULT_GOLOMB_M, writer) {

    this->m = m;
    this->outputFileName = outFileName;
    this->predictor = pred;
}

GEncoder::~GEncoder() {
    golomb.flush();
    golomb.~Golomb();
    writer.~BitStream();
}

int GEncoder::calculate_m(std::vector<short>& values, int blockSize) {
    // Combine all values
    int sum = 0;
    for (auto& value : values)
        sum += abs(value);

    // Transform the sum in [0,1]
    double p = (double)sum / (double)blockSize;

    // Calculate m using the formula: m = ceil(1 - log(1 + p) / log(p))
    double aux = (log((1 + p)) / log(p));
    int m = std::ceil(aux);
    // guarantee that minimum value is 1
    return std::max(1, m);
}

void GEncoder::quantize_samples(std::vector<short>& inSamples) {
    for (auto& sample : inSamples) {
        sample >>= fileStruct.quantizationBits;
    }
}

void GEncoder::write_file() {
    // Write file header
    writer.writeNBits(fileStruct.blockSize, 16);
    writer.writeNBits(fileStruct.sampleRate, 16);
    writer.writeNBits(fileStruct.nFrames, 32);
    writer.writeNBits(fileStruct.nChannels, 8);
    writer.writeNBits(fileStruct.quantizationBits, 8);
    writer.writeNBits(fileStruct.lossy, 2);

    // Write Blocks (data)
    int count = 0;
    for (auto& block : fileStruct.blocks) {
        // Write Block header
        writer.writeNBits(block.m, 8);
        writer.writeNBits(block.predictor, 4);

        // Write Block data
        cout << " - Writing Block " << count++
             << " to file with m = " << unsigned(block.m) << endl;
        this->golomb.setM(block.m);
        for (auto sample : block.data) {
            golomb.encode(sample);
        }
    }
}

Block GEncoder::process_block(std::vector<short>& block) {
    // change this to benchmark the various predictors in automatic
    PREDICTOR pred = PREDICT1;

    Block encodedBlock;
    encodedBlock.predictor = pred;

    // Mono treatment
    for (int i = 0; i < (int)block.size(); i++) {
        // take into account that when starting, arr index < 0 are 0
        int a = (i - 1) < 0 ? 0 : block.at(i - 1);
        int b = (i - 2) < 0 ? 0 : block.at(i - 2);
        int c = (i - 3) < 0 ? 0 : block.at(i - 3);
        encodedBlock.data.push_back(predict1(a, b, c));
    }

    // Use attributed m or calculate one
    int bM = m;
    if (bM < 1)
        bM = calculate_m((block), fileStruct.blockSize);
    encodedBlock.m = bM;

    return encodedBlock;
}

void GEncoder::encode_file(File file, std::vector<short>& inSamples,
                           size_t nBlocks) {
    this->fileStruct = file;
    // Divide in blocks and process each one
    for (int i = 0; i < (int)nBlocks; i++) {
        std::vector<short> block;
        for (int j = 0; j < file.blockSize; j++) {
            block.push_back(inSamples[i * file.blockSize + j]);
        }
        Block encodedBlock = process_block(block);

        // Add encoded block to file
        fileStruct.blocks.push_back(encodedBlock);
    }

    cout << "All blocks encoded. Writing to file" << endl;

    write_file();
}

/*
##############################################################################
###################         Golomb Decoder Code            ###################
##############################################################################
*/

GDecoder::GDecoder(std::string inFileName)
    : reader('r', inFileName), golomb(DEFAULT_GOLOMB_M, reader) {

    this->inputFileName = inFileName;
}

GDecoder::~GDecoder() {
    golomb.flush();
    golomb.~Golomb();
    reader.~BitStream();
}

File GDecoder::read_file() {
    cout << "Reading file " << inputFileName;

    File f;
    // Read file header
    f.blockSize = reader.readNBits(16);
    f.sampleRate = reader.readNBits(16);
    f.nFrames = reader.readNBits(32);
    f.nChannels = reader.readNBits(8);
    f.quantizationBits = reader.readNBits(8);
    f.lossy = reader.readNBits(2);

    // Write Blocks (data)
    size_t nBlocks{static_cast<size_t>(
        ceil(static_cast<double>(f.nFrames) / f.blockSize))};

    cout << " with " << unsigned(nBlocks) << " blocks" << endl;
    for (unsigned long bId; bId < nBlocks; bId++) {
        Block block;
        // Read Block header
        block.m = reader.readNBits(8);
        block.predictor = static_cast<PREDICTOR>(reader.readNBits(4));

        // Read Block data
        this->golomb.setM(block.m);
        //cout << " - Reading Block " << unsigned(bId)
        //     << " with m = " << unsigned(block.m) << endl;
        for (uint16_t i = 0; i < f.blockSize; i++)
            block.data.push_back((short)golomb.decode());
    }

    return f;
}

std::vector<short> GDecoder::decode_block(Block block) {}

std::vector<short> GDecoder::decode_file(File& f) {}