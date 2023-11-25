#include "golomb_codec.h"

int predict1(int a, int b, int c) {
    return 1 * a - 3 * b + c;
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
    double aux = 1 - (log((1 + p)) / log(p));
    int m = std::ceil(aux);
    // returns 1 if m < 1 else m
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
    writer.writeNBits(fileStruct.nChannels, 8);
    writer.writeNBits(fileStruct.sampleRate, 16);
    writer.writeNBits(fileStruct.nFrames, 16);
    writer.writeNBits(fileStruct.quantizationBits, 16);

    // Write Blocks (data)
    int count = 0;
    for (auto& block : fileStruct.blocks) {
        // Write Block header
        writer.writeNBits(block.m, 8);
        writer.writeNBits(block.predictor, 8);
        // padding should be added if header size is not multiple of 8

        // Write Block data
        cout << " - Writing Block " << count++ << " to file" << endl;
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
        bM = calculate_m((encodedBlock.data), fileStruct.blockSize);
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

GDecoder::GDecoder(/* args */) {}

GDecoder::~GDecoder() {}