#include "image_codec.h"

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

int GEncoder::test_calculate_m(std::vector<std::vector<uint8_t>>& values) {
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

// TODO - Values of pixels are unsigned, don't need to be converted to abs
int GEncoder::calculate_m(std::vector<std::vector<uint8_t>>& values) {
    /* Calculate alpha */
    double alpha = 1.0;
    if (!values.empty() && !values.at(0).empty()) {
        std::vector<uint8_t> img_values;

        // Flatten vector by rows
        for (std::vector<uint8_t>& row : values) {
            img_values.insert(img_values.end(), row.begin(), row.end());
        }

        // Calculate pixel values mean for the block
        double mean = static_cast<double>(std::accumulate(
                            img_values.begin(), img_values.end(), 0)) /
                        img_values.size();
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
    writer.writeNBits(fileStruct.type, BITS_FILE_TYPE);
    writer.writeNBits(fileStruct.blockSize, BITS_BLOCK_SIZE);
    //writer.writeNBits(fileStruct.nFrames, BITS_N_FRAMES);
    writer.writeNBits(fileStruct.chroma, BITS_CHROMA);
    writer.writeNBits(fileStruct.width, BITS_WIDTH);
    writer.writeNBits(fileStruct.height, BITS_HEIGHT);
    writer.writeString(fileStruct.fps);
    writer.writeNBits(fileStruct.approach, BITS_APPROACH);
    writer.writeNBits(fileStruct.lossy, BITS_LOSSY);
}

// TODO
void GEncoder::write_file_frame(FrameSegment& frame) {
    writer.writeNBits(frame.type, BITS_FRAME_TYPE);

    // BLOCKS ARE WRITTEN IN THE ENCODE_FILE FUNCTION

    // for (Block& block : frame.blocks) {
    //     write_file_block(block, 0, 0);
    // }
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

    golomb.setM(block.m); 

    // Write Block data
    std::cout << "\r" << std::flush << " - Writing Block " << std::setw(3)
              << blockId++ << "/" << std::setw(3) << nBlocks
              << " to file with m = " << std::setw(3) << unsigned(block.m)
              << ", p = " << std::setw(1) << unsigned(block.predictor);

    // TODO - Done
    for (std::vector<uint8_t>& row : block.data) {
        for (uint8_t& pixel : row) {
            golomb.encode(pixel);
        }
    }
}

// TODO
Block GEncoder::process_block(std::vector<std::vector<uint8_t>>& block, int blockId,
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

    // For each pixel in the block, calculate prediction and error
    for (int i = 0; i < static_cast<int>(block.size()); i++) { 
        std::vector<uint8_t> rowErrors;
        for (int j = 0; j < static_cast<int>(block[i].size()); j++) { 
            int prediction = predictorClass.predict(pred, block, i, j); // (i, j) -> (row, col)
            int error = block[i][j] - prediction;
            rowErrors.push_back(static_cast<uint8_t>(error));
        }
        encodedBlock.data.push_back(rowErrors);
    }

    // Use attributed m or calculate one
    int bM = m;
    if (bM < 1)
        bM = calculate_m(encodedBlock.data);
    encodedBlock.m = bM;

    // TODO - Falta frame type?

    return encodedBlock;
}

// TODO
void GEncoder::encode_file_header(File file) {
    this->fileStruct = file;
    this->golomb.setApproach(fileStruct.approach);

    write_file_header();

    /*if(frameId == 0){
    write_file_header();}

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

    frameId++;

    if (frameId == nFrames) {
        std::cout << "All frames encoded..." << std::endl;
    }*/
}

void GEncoder::encode_frame(Frame frame, size_t nBlocks){


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