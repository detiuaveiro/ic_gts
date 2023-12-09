#include "audio_codec.h"

/*
##############################################################################
###################           Predictor Class              ###################
##############################################################################
*/

std::string get_type_string(PREDICTOR_TYPE type) {
    std::string predText = "AUTOMATIC (0)";
    if (type == PREDICT1)
        predText = "PREDICT1 (1)";
    else if (type == PREDICT2)
        predText = "PREDICT2 (2)";
    else if (type == PREDICT3)
        predText = "PREDICT3 (3)";
    else
        predText = "UNKNOWN";
    return predText;
}

Predictor::Predictor() {}

Predictor::~Predictor() {}

int Predictor::predict1(int a1) {
    return a1;
}

int Predictor::predict2(int a1, int a2) {
    return (2 * a1) - a2;
}

int Predictor::predict3(int a1, int a2, int a3) {
    return (3 * a1) - (3 * a2) + a3;
}

double Predictor::calculate_entropy(PREDICTOR_TYPE type,
                                    std::vector<short>& samples) {
    int total_predictions = 0;
    std::vector<int> predictions;

    // Predict based on the given type and count occurrences of predictions
    for (size_t i = 0; i < samples.size(); ++i) {
        int prediction;
        if (type == PREDICT1) {
            prediction = predict(type, samples, i);
        } else if (type == PREDICT2) {
            prediction = predict(type, samples, i);
        } else if (type == PREDICT3) {
            prediction = predict(type, samples, i);
        } else {
            cerr << "Error: Unknown Predictor type encountered" << endl;
            exit(2);
        }
        predictions.push_back(prediction);
        total_predictions++;
    }

    // Calculate probability distribution and entropy
    double entropy = 0.0;
    for (int value : predictions) {
        double probability =
            std::count(predictions.begin(), predictions.end(), value) /
            static_cast<double>(total_predictions);
        entropy -= probability * std::log2(probability);
    }

    return entropy;
}

int Predictor::predict(PREDICTOR_TYPE type, std::vector<short>& samples,
                       int index) {
    if (!check_type(type) || type == AUTOMATIC) {
        cerr << "Error: Unknown/Invalid Predictor " << unsigned(type)
             << " encountered while decoding Block" << endl;
        exit(2);
    }

    int a1, a2, a3;

    a1 = (index - 1) < 0 ? 0 : samples.at(index - 1);
    a2 = (index - 2) < 0 ? 0 : samples.at(index - 2);
    a3 = (index - 3) < 0 ? 0 : samples.at(index - 3);

    if (type == PREDICT1)
        return predict1(a1);
    else if (type == PREDICT2)
        return predict2(a1, a2);
    else
        return predict3(a1, a2, a3);
}

bool Predictor::check_type(PREDICTOR_TYPE type) {
    if (type == AUTOMATIC || type == PREDICT1 || type == PREDICT2 ||
        type == PREDICT3)
        return true;
    return false;
}

PREDICTOR_TYPE Predictor::benchmark(std::vector<short>& samples) {
    double min_entropy = std::numeric_limits<double>::max();
    PREDICTOR_TYPE best_predictor = AUTOMATIC;

    double entropy1 = calculate_entropy(PREDICT1, samples);
    if (entropy1 < min_entropy) {
        min_entropy = entropy1;
        best_predictor = PREDICT1;
    }

    double entropy2 = calculate_entropy(PREDICT2, samples);
    if (entropy2 < min_entropy) {
        min_entropy = entropy2;
        best_predictor = PREDICT2;
    }

    double entropy3 = calculate_entropy(PREDICT3, samples);
    if (entropy3 < min_entropy) {
        min_entropy = entropy3;
        best_predictor = PREDICT3;
    }

    return best_predictor;
}

/*
##############################################################################
###################         Golomb Encoder Code            ###################
##############################################################################
*/

GEncoder::GEncoder(std::string outFileName, int m, PREDICTOR_TYPE pred)
    : writer('w', outFileName), golomb(DEFAULT_GOLOMB_M, writer) {

    this->m = m;
    this->outputFileName = outFileName;

    if (!predictorClass.check_type(pred)) {
        cerr << "Error: Unknown Predictor " << unsigned(pred)
             << " encountered while creating Encoder" << endl;
        exit(2);
    }
    this->predictor = pred;
}

GEncoder::~GEncoder() {
    writer.~BitStream();
}

void GEncoder::setFile(File file) {
    this->fileStruct = file;
}

int GEncoder::test_calculate_m(std::vector<short>& values) {
    return calculate_m(values);
}

std::vector<unsigned short> GEncoder::test_abs_value_vector(
    std::vector<short>& values) {
    return abs_value_vector(values);
}

std::vector<unsigned short> GEncoder::abs_value_vector(
    std::vector<short>& values) {
    std::vector<unsigned short> result;
    result.reserve(values.size());

    for (const auto& elem : values) {
        result.push_back(std::abs(elem));
    }

    return result;
}

int GEncoder::calculate_m(std::vector<short>& values) {
    /* Calculate alpha */
    double alpha = 1.0;
    if (!values.empty()) {
        std::vector<unsigned short> abs_values = abs_value_vector(values);
        double mean = static_cast<double>(std::accumulate(
                          abs_values.begin(), abs_values.end(), 0)) /
                      abs_values.size();
        alpha = exp(-1.0 / mean);  // Calculate alpha using mean
    }
    // Calculate 'm' based on the formula
    double aux = (-1 / log(alpha));
    int m = std::ceil(aux);

    // guarantee that minimum value is 1
    m = std::max(1, m);

    return std::min(m, 16383);  // cap golomb max size (14 bits)
}

void GEncoder::write_file_header() {
    writer.writeNBits(fileStruct.blockSize, BITS_BLOCK_SIZE);
    writer.writeNBits(fileStruct.sampleRate, BITS_SAMPLE_RATE);
    writer.writeNBits(fileStruct.nFrames, BITS_N_FRAMES);
    writer.writeNBits(fileStruct.nChannels, BITS_N_CHANNELS);
    writer.writeNBits(fileStruct.bitRate, BITS_BIT_RATE);
    writer.writeNBits(fileStruct.lossy, BITS_LOSSY);
    writer.writeNBits(fileStruct.approach, BITS_APPROACH);
}

void GEncoder::write_file_block(Block& block, int blockId, int nBlocks) {
    golomb.setApproach(fileStruct.approach);

    // check if block size is correct
    if (block.data.size() != fileStruct.blockSize) {
        cerr << "Error: Block size mismatch" << endl;
        exit(2);
    }

    // Write Block header
    writer.writeNBits(block.m, BITS_M);
    writer.writeNBits(block.predictor, BITS_PREDICTOR);

    golomb.setM(block.m);  // DONT FORGET THIS

    // Write Block data
    std::cout << "\r" << std::flush << " - Writing Block " << std::setw(3)
              << blockId++ << "/" << std::setw(3) << nBlocks
              << " to file with m = " << std::setw(3) << unsigned(block.m)
              << ", p = " << std::setw(1) << unsigned(block.predictor);

    for (short& sample : block.data)
        golomb.encode(sample);
}

Block GEncoder::process_block(std::vector<short>& block, int blockId,
                              int nBlocks) {

    PREDICTOR_TYPE pred = predictor;
    if (pred == AUTOMATIC) {
        std::cout << "\r" << std::flush << " - "
                  << "Benchmarking predictor for Block " << std::setw(4)
                  << blockId << "/" << std::setw(4) << nBlocks;
        pred = predictorClass.benchmark(block);
    }

    Block encodedBlock;
    encodedBlock.predictor = pred;

    for (int i = 0; i < (int)block.size(); i++) {
        int prediction = predictorClass.predict(pred, block, i);
        int error = block.at(i) - prediction;

        encodedBlock.data.push_back(error);
    }

    // Use attributed m or calculate one
    int bM = m;
    if (bM < 1)
        bM = calculate_m(encodedBlock.data);
    encodedBlock.m = bM;

    return encodedBlock;
}

void GEncoder::encode_file(File file, std::vector<short>& inSamples,
                           size_t nBlocks) {
    this->fileStruct = file;
    this->golomb.setApproach(fileStruct.approach);

    write_file_header();

    std::cout << "\nStarting encoding phase..." << std::endl;
    // Divide in blocks and process each one
    for (int i = 0; i < (int)nBlocks; i++) {
        std::vector<short> block;
        for (int j = 0; j < file.blockSize; j++)
            block.push_back(inSamples[i * file.blockSize + j]);

        Block encodedBlock = process_block(block, i + 1, nBlocks);
        write_file_block(encodedBlock, i + 1, nBlocks);
    }

    std::cout << "\nEncoding ended, all data written to file\n" << endl;
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
    reader.~BitStream();
}

File& GDecoder::get_file() {
    return fileStruct;
}

int GDecoder::read_file_header() {
    if (!headerRead) {
        // Read file header
        fileStruct.blockSize = reader.readNBits(BITS_BLOCK_SIZE);
        fileStruct.sampleRate = reader.readNBits(BITS_SAMPLE_RATE);
        fileStruct.nFrames = reader.readNBits(BITS_N_FRAMES);
        fileStruct.nChannels = reader.readNBits(BITS_N_CHANNELS);
        fileStruct.bitRate = reader.readNBits(BITS_BIT_RATE);
        fileStruct.lossy = reader.readNBits(BITS_LOSSY);
        fileStruct.approach =
            static_cast<APPROACH>(reader.readNBits(BITS_APPROACH));

        headerRead = true;

        if (!check_approach(fileStruct.approach)) {
            cerr << "Error: Invalid approach type " << fileStruct.approach
                 << endl;
            exit(1);
        }

        golomb.setApproach(fileStruct.approach);
    }

    int nBlocks{static_cast<int>(
        ceil(static_cast<double>(fileStruct.nFrames) / fileStruct.blockSize)) * fileStruct.nChannels};

    return nBlocks;
}

Block GDecoder::read_file_block(int blockId, int nBlocks) {
    Block block;

    // Read Block header
    block.m = reader.readNBits(BITS_M);
    block.predictor =
        static_cast<PREDICTOR_TYPE>(reader.readNBits(BITS_PREDICTOR));

    // Read Block data
    this->golomb.setM(block.m);
    std::cout << " - Reading Block " << std::setw(3) << blockId << "/"
              << std::setw(3) << nBlocks << " with m = " << std::setw(3)
              << unsigned(block.m) << ", predictor = " << std::setw(1)
              << unsigned(block.predictor) << endl;

    // check m
    if (block.m < 1) {
        cerr << "Error: Encountered invalid m = " << unsigned(block.m) << endl;
        exit(2);
    }

    for (uint16_t i = 0; i < fileStruct.blockSize; i++) {
        int data = golomb.decode();
        block.data.push_back((short)data);
    }

    return block;
}

std::vector<short> GDecoder::decode_block(Block& block) {
    std::vector<short> samples;
    PREDICTOR_TYPE pred = static_cast<PREDICTOR_TYPE>(block.predictor);

    for (int i = 0; i < (int)block.data.size(); i++) {
        int prediction = predictorClass.predict(pred, samples, i);
        // error + prediction
        int sample = block.data.at(i) + prediction;
        samples.push_back(sample);
    }

    return samples;
}

std::vector<short> GDecoder::decode_file() {
    std::vector<short> outSamples;

    int nBlocks = read_file_header();

    std::cout << "Decoding file with " << unsigned(nBlocks) << " Blocks..."
              << endl;

    for (int bId = 0; bId < nBlocks; bId++) {
        Block block = read_file_block(bId + 1, nBlocks);

        std::cout << " - Decoding Block: " << bId + 1 << "\r" << std::flush;
        std::vector<short> blockSamples = decode_block(block);
        outSamples.insert(outSamples.end(), blockSamples.begin(),
                          blockSamples.end());
    }
    std::cout << "\nAll Blocks read and decoded\n" << endl;

    return outSamples;
}