#include "golomb_codec.h"

/*
##############################################################################
###################           Predictor Class              ###################
##############################################################################
*/

Predictor::Predictor() {}

Predictor::~Predictor() {}

PREDICTOR_TYPE Predictor::benchmark(std::vector<short> samples) {}

int Predictor::predict(PREDICTOR_TYPE type, std::vector<short> samples) {
    if (!check_type(type)) {
        cerr << "Error: Unknown Predictor " << unsigned(type)
             << " encountered while decoding Block" << endl;
        exit(2);
    }

    int size = (int)samples.size();
    if (type == PREDICT1) {
        int a1 = (size - 1) < 0 ? 0 : samples.at(size - 1);
        return predict1(a1);
    } else if (type == PREDICT2) {
        int a1 = (size - 1) < 0 ? 0 : samples.at(size - 1);
        int a2 = (size - 2) < 0 ? 0 : samples.at(size - 2);
        return predict2(a1, a2);
    } else if (type == PREDICT3) {
        int a1 = (size - 1) < 0 ? 0 : samples.at(size - 1);
        int a2 = (size - 2) < 0 ? 0 : samples.at(size - 2);
        int a3 = (size - 3) < 0 ? 0 : samples.at(size - 3);
        return predict3(a1, a2, a3);
    }
}

int Predictor::predict1(int a1) {
    return a1;
}

int Predictor::predict2(int a1, int a2) {
    return (2 * a1) - a2;
}

int Predictor::predict3(int a1, int a2, int a3) {
    return (3 * a1) - (3 * a2) + a3;
}

bool Predictor::check_type(PREDICTOR_TYPE type) {
    if (type == PREDICT1 || type == PREDICT2 || type == PREDICT3)
        return true;
    return false;
}

/*
##############################################################################
###################         Golomb Encoder Code            ###################
##############################################################################
*/

GEncoder::GEncoder(std::string outFileName, int m = -1,
                   PREDICTOR_TYPE pred = AUTOMATIC)
    : writer('w', outFileName), golomb(DEFAULT_GOLOMB_M, writer) {

    this->m = m;
    this->outputFileName = outFileName;

    if (!predictorClass.check_type(pred)) {
        cerr << "Error: Unknown Predictor " << unsigned(pred)
             << " encountered while decoding Block" << endl;
        exit(2);
    }
    this->predictor = pred;
}

GEncoder::~GEncoder() {
    golomb.~Golomb();
    writer.~BitStream();
}

int GEncoder::test_calculate_m(std::vector<short>& values, int blockSize) {
    return calculate_m(values, blockSize);
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
    writer.writeNBits(fileStruct.nChannels, 4);
    writer.writeNBits(fileStruct.quantizationBits, 8);
    writer.writeNBits(fileStruct.lossy, 2);

    // Write Blocks (data)
    cout << "Writing Blocks to file..." << endl;

    int count = 0;
    for (auto& block : fileStruct.blocks) {
        // Write Block header
        writer.writeNBits(block.m, 8);
        writer.writeNBits(block.predictor, 4);

        // Write Block data
        cout << " - Writing Block " << count++
             << " to file with m = " << unsigned(block.m)
             << " and predictor = " << block.predictor << endl;
        this->golomb.setM(block.m);
        for (auto sample : block.data) {
            golomb.encode(sample);
        }
    }
}

Block GEncoder::process_block(std::vector<short>& block) {

    PREDICTOR_TYPE pred = predictor;
    if (pred == AUTOMATIC)
        pred = predictorClass.benchmark(block);

    Block encodedBlock;
    encodedBlock.predictor = pred;

    // only works for Mono
    for (int i = 0; i < (int)block.size(); i++) {
        int prediction = predictorClass.predict(pred, block);
        encodedBlock.data.push_back(prediction);
    }

    // Use attributed m or calculate one (improve this part)
    int bM = m;
    if (bM < 1)
        bM = calculate_m((block), fileStruct.blockSize);
    encodedBlock.m = bM;

    return encodedBlock;
}

void GEncoder::encode_file(File file, std::vector<short>& inSamples,
                           size_t nBlocks) {
    this->fileStruct = file;

    // Probably add quantization

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
    golomb.~Golomb();
    reader.~BitStream();
}

File& GDecoder::read_file() {
    cout << "Reading file " << inputFileName;

    // Read file header
    fileStruct.blockSize = reader.readNBits(16);
    fileStruct.sampleRate = reader.readNBits(16);
    fileStruct.nFrames = reader.readNBits(32);
    fileStruct.nChannels = reader.readNBits(4);
    fileStruct.quantizationBits = reader.readNBits(8);
    fileStruct.lossy = reader.readNBits(2);

    // Write Blocks (data)
    size_t nBlocks{static_cast<size_t>(
        ceil(static_cast<double>(fileStruct.nFrames) / fileStruct.blockSize))};

    cout << " with " << unsigned(nBlocks) << " blocks" << endl;
    for (unsigned long bId = 0; bId < nBlocks; bId++) {
        Block block;
        // Read Block header
        block.m = reader.readNBits(8);
        block.predictor = static_cast<PREDICTOR_TYPE>(reader.readNBits(4));

        // Read Block data
        this->golomb.setM(block.m);
        cout << " - Reading Block " << unsigned(bId)
             << " with m = " << unsigned(block.m)
             << " and predictor = " << unsigned(block.predictor) << endl;
        for (uint16_t i = 0; i < fileStruct.blockSize; i++)
            block.data.push_back((short)golomb.decode());

        fileStruct.blocks.push_back(block);
    }

    return fileStruct;
}

std::vector<short> GDecoder::decode_block(Block& block) {
    std::vector<short> samples;
    PREDICTOR_TYPE pred = static_cast<PREDICTOR_TYPE>(block.predictor);

    for (int i = 0; i < (int)block.data.size(); i++)
        samples.push_back(predictorClass.predict(pred, block.data));

    return samples;
}

std::vector<short> GDecoder::decode_file() {
    std::vector<short> outSamples;
    cout << "Decoding file with " << unsigned(fileStruct.blocks.size())
         << " Blocks" << endl;
    int count = 0;
    for (Block& block : fileStruct.blocks) {
        cout << " - Decoding Block: " << count++ << endl;
        std::vector<short> blockSamples = decode_block(block);
        outSamples.insert(outSamples.end(), blockSamples.begin(),
                          blockSamples.end());
    }

    return outSamples;
}