#include "golomb_codec.h"

/*
##############################################################################
###################           Predictor Class              ###################
##############################################################################
*/

std::string get_type_string(PREDICTOR_TYPE type) {
    std::string predText = "AUTOMATIC";
    if (type == AUTOMATIC)
        predText = "AUTOMATIC (0)";
    else if (type == PREDICT1)
        predText = "PREDICT1 (1)";
    else if (type == PREDICT2)
        predText = "PREDICT2 (2)";
    else if (type == PREDICT3)
        predText = "PREDICT3 (3)";
    else
        predText = "UNKNOWN";
    return predText;
}

Predictor::Predictor(int nChannels) {
    this->nChannels = nChannels;
}

Predictor::Predictor() {}

Predictor::~Predictor() {}

void Predictor::set_nChannels(int nChannels) {
    this->nChannels = nChannels;
}
int Predictor::get_nChannels() {
    return this->nChannels;
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

double Predictor::calculateEntropy(PREDICTOR_TYPE type,
                                   std::vector<short>& samples) {
    int total_predictions = 0;
    std::vector<int> predictions;

    // Predict based on the given type and count occurrences of predictions
    for (size_t i = 3; i < samples.size(); ++i) {
        int prediction;
        if (type == PREDICT1) {
            prediction = predict(type, samples, i);
        } else if (type == PREDICT2) {
            prediction = predict(type, samples, i);
        } else {
            prediction = predict(type, samples, i);
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

int Predictor::predict(PREDICTOR_TYPE type, std::vector<short> samples,
                       int index) {
    if (!check_type(type)) {
        cerr << "Error: Unknown Predictor " << unsigned(type)
             << " encountered while decoding Block" << endl;
        exit(2);
    }

    // index and nChannels will be used to interact only with left or right channels
    nChannels =
        nChannels % nChannels;  // dummy calculation for compiler warnings

    // with more than one channel use correlation (with MID and SIDE channels) or
    //  or encode each one separately (only save the difference)

    if (type == PREDICT1) {
        int a1 = (index - 1) < 0 ? 0 : samples.at(index - 1);
        return predict1(a1);
    } else if (type == PREDICT2) {
        int a1 = (index - 1) < 0 ? 0 : samples.at(index - 1);
        int a2 = (index - 2) < 0 ? 0 : samples.at(index - 2);
        return predict2(a1, a2);
    } else {
        int a1 = (index - 1) < 0 ? 0 : samples.at(index - 1);
        int a2 = (index - 2) < 0 ? 0 : samples.at(index - 2);
        int a3 = (index - 3) < 0 ? 0 : samples.at(index - 3);
        return predict3(a1, a2, a3);
    }
}

bool Predictor::check_type(PREDICTOR_TYPE type) {
    if (type == AUTOMATIC || type == PREDICT1 || type == PREDICT2 ||
        type == PREDICT3)
        return true;
    return false;
}

PREDICTOR_TYPE Predictor::benchmark(std::vector<short> samples) {
    double min_entropy = std::numeric_limits<double>::max();
    PREDICTOR_TYPE best_predictor = AUTOMATIC;

    double entropy1 = calculateEntropy(PREDICT1, samples);
    if (entropy1 < min_entropy) {
        min_entropy = entropy1;
        best_predictor = PREDICT1;
    }

    double entropy2 = calculateEntropy(PREDICT2, samples);
    if (entropy2 < min_entropy) {
        min_entropy = entropy2;
        best_predictor = PREDICT2;
    }

    double entropy3 = calculateEntropy(PREDICT3, samples);
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

void GEncoder::quantize_samples(std::vector<short>& inSamples) {
    for (auto& sample : inSamples) {
        sample >>= fileStruct.quantizationBits;
    }
}

void GEncoder::write_file() {

    // Write file header_values
    writer.writeNBits(fileStruct.blockSize, BITS_BLOCK_SIZE);
    writer.writeNBits(fileStruct.sampleRate, BITS_SAMPLE_RATE);
    writer.writeNBits(fileStruct.nFrames, BITS_N_FRAMES);
    writer.writeNBits(fileStruct.nChannels, BITS_N_CHANNELS);
    writer.writeNBits(fileStruct.quantizationBits, BITS_QUANTIZATION_BITS);
    writer.writeNBits(fileStruct.lossy, BITS_LOSSY);
    writer.writeNBits(fileStruct.approach, BITS_APPROACH);
    golomb.setApproach(fileStruct.approach);

    // Write Blocks (data)
    cout << "\nWriting data to file..." << endl;

    int count = 1;
    for (auto& block : fileStruct.blocks) {
        // check if block size is correct
        if (block.data.size() != fileStruct.blockSize) {
            cerr << "Error: Block size mismatch" << endl;
            exit(2);
        }

        // Write Block header
        writer.writeNBits(block.m, BITS_M);
        writer.writeNBits(block.predictor, BITS_PREDICTOR);

        // Write Block data
        cout << " - Writing Block " << std::setw(3) << count++ << "/"
             << std::setw(3) << fileStruct.blocks.size()
             << " to file with m = " << std::setw(3) << unsigned(block.m)
             << ", p = " << std::setw(3) << unsigned(block.predictor) << "\r"
             << std::flush;

        for (short& sample : block.data) {
            if (sample != 10) {
                cout << "Error: encountered sample different than 10 sample "
                        "with value: "
                     << sample << endl;
                exit(3);
            }
            golomb.encode(sample);
        }
    }
    std::cout << "\nAll data written to file\n\n";
}

Block GEncoder::process_block(std::vector<short>& block, int blockId,
                              int nBlocks) {

    PREDICTOR_TYPE pred = predictor;
    if (pred == AUTOMATIC) {
        std::cout << " - "
                  << "Benchmarking predictor for Block " << std::setw(3)
                  << blockId + 1 << "/" << nBlocks << "\r" << std::flush;
        pred = predictorClass.benchmark(block);
    }

    Block encodedBlock;
    encodedBlock.predictor = pred;

    // only works for Mono
    for (int i = 0; i < (int)block.size(); i++) {
        //int prediction = predictorClass.predict(pred, block, i);
        //int error = block.at(i) - prediction;

        encodedBlock.data.push_back(10);  //error);
    }

    // Use attributed m or calculate one
    int bM = m;
    if (bM < 1)
        bM = calculate_m((encodedBlock.data));
    encodedBlock.m = bM;

    return encodedBlock;
}

void GEncoder::encode_file(File file, std::vector<short>& inSamples,
                           size_t nBlocks) {
    this->fileStruct = file;
    this->predictorClass.set_nChannels(file.nChannels);

    // Probably add quantization

    std::cout << "Entering encoding phase" << std::endl;
    // Divide in blocks and process each one
    for (int i = 0; i < (int)nBlocks; i++) {
        std::vector<short> block;
        for (int j = 0; j < file.blockSize; j++)
            block.push_back(inSamples[i * file.blockSize + j]);

        Block encodedBlock = process_block(block, i, nBlocks);

        // Add encoded block to file
        fileStruct.blocks.push_back(encodedBlock);
    }
    if (predictor == AUTOMATIC)
        std::cout << "\n";

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
    fileStruct.blockSize = reader.readNBits(BITS_BLOCK_SIZE);
    fileStruct.sampleRate = reader.readNBits(BITS_SAMPLE_RATE);
    fileStruct.nFrames = reader.readNBits(BITS_N_FRAMES);
    fileStruct.nChannels = reader.readNBits(BITS_N_CHANNELS);
    fileStruct.quantizationBits = reader.readNBits(BITS_QUANTIZATION_BITS);
    fileStruct.lossy = reader.readNBits(BITS_LOSSY);
    fileStruct.approach =
        static_cast<APPROACH>(reader.readNBits(BITS_APPROACH));

    // Write Blocks (data)
    int nBlocks{static_cast<int>(
        ceil(static_cast<double>(fileStruct.nFrames) / fileStruct.blockSize))};
    cout << " with " << unsigned(nBlocks) << " blocks" << endl;

    cout << "\nMusic Processing information: \n"
         << " - Block Size: " << fileStruct.blockSize
         << "\n - Number of Channels: " << unsigned(fileStruct.nChannels)
         << "\n - Sample Rate: " << fileStruct.sampleRate
         << "\n - Total Number of Frames: " << unsigned(fileStruct.nFrames)
         << "\n - Number of Blocks: " << nBlocks
         << "\n - Golomb Approach: " << approach_to_string(fileStruct.approach)
         << "\n - Encode type: " << (fileStruct.lossy ? "lossy" : "lossless")
         << "\n"
         << endl;

    if (!check_approach(fileStruct.approach)) {
        cerr << "Error: Invalid approach type " << fileStruct.approach << endl;
        exit(1);
    }
    golomb.setApproach(fileStruct.approach);

    for (int bId = 0; bId < nBlocks; bId++) {
        Block block;
        // Read Block header
        block.m = reader.readNBits(BITS_M);
        block.predictor =
            static_cast<PREDICTOR_TYPE>(reader.readNBits(BITS_PREDICTOR));

        // Read Block data
        this->golomb.setM(block.m);
        cout << " - Reading Block " << std::setw(3) << bId + 1
             << " with m = " << std::setw(3) << unsigned(block.m)
             << " and predictor = " << std::setw(3) << unsigned(block.predictor)
             << endl;

        // check m
        if (block.m < 1) {
            cerr << "Error: Encountered invalid m = " << unsigned(block.m)
                 << endl;
            exit(2);
        }

        for (uint16_t i = 0; i < fileStruct.blockSize; i++) {
            int decoded = golomb.decode();
            if (decoded != 10) {
                cout << "Error: encountered sample different than 10 with "
                        "value: "
                     << decoded << ", index = " << i << endl;
                exit(3);
            }
            block.data.push_back((short)decoded);
            //block.data.push_back((short)golomb.decode());
        }

        fileStruct.blocks.push_back(block);
    }

    return fileStruct;
}

std::vector<short> GDecoder::decode_block(Block& block) {
    std::vector<short> samples;
    PREDICTOR_TYPE pred = static_cast<PREDICTOR_TYPE>(block.predictor);

    for (int i = 0; i < (int)block.data.size(); i++)
        samples.push_back(predictorClass.predict(pred, block.data, i));

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