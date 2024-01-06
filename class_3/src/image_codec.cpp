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

int GEncoder::calculate_m(Mat& values) {
    /* Calculate alpha */
    double alpha = 1.0;
    if (!values.empty()) {
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

    return std::min(m, pow(2, BITS_M) - 1);  // cap golomb max size
}

void GEncoder::write_file_header() {
    writer.writeNBits(fileStruct.type, BITS_FILE_TYPE);
    writer.writeNBits(fileStruct.blockSize, BITS_BLOCK_SIZE);
    writer.writeNBits(fileStruct.nFrames, BITS_N_FRAMES);
    writer.writeNBits(fileStruct.chroma, BITS_CHROMA);
    writer.writeNBits(fileStruct.width, BITS_WIDTH);
    writer.writeNBits(fileStruct.height, BITS_HEIGHT);
    writer.writeString(fileStruct.fps, BITS_FPS);
    writer.writeNBits(fileStruct.approach, BITS_APPROACH);
    writer.writeNBits(fileStruct.lossy, BITS_LOSSY);
}

void GEncoder::write_frame_header(FrameSegment& frame) {
    writer.writeNBits(frame.type, BITS_FRAME_TYPE);
    writer.writeNBits(frame.predictor, BITS_PREDICTOR);
    writer.writeNBits(frame.m, BITS_M);
    golomb.setM(frame.m);
}

void GEncoder::write_file_block(Block& block, int blockId, int nBlocks,
                                FrameSegment& frame) {
    golomb.setApproach(fileStruct.approach);

    // Write Frame type
    if (frame.type == P)
        writer.writeNBits(block.type, BITS_FRAME_TYPE);

    golomb.setM(frame.m);

    // Write Block data
    /*std::cout << "\r" << std::flush << " - Writing Block " << std::setw(3)
              << blockId++ << "/" << std::setw(3) << nBlocks
              << " to file with m = " << std::setw(3) << unsigned(block.m)
              << ", p = " << std::setw(1) << unsigned(block.predictor);*/

    for (uint8_t& pixel : block.data) {
        golomb.encode(pixel);
    }
}

Block GEncoder::process_block(Mat& block, int blockId, int nBlocks,
                              PREDICTOR_TYPE pred, FrameSegment& frame) {

    Block encodedBlock;

    // If it's an inter-frame, no need to test data
    if (frame.type == I) {
        encodedBlock.type = I;
        encodedBlock.data = block;
        return encodedBlock
    }

    // Predict the data and create a new predicted object
    cv::Mat predictBlock = cv::Mat::zeros(block.size(), block.type());
    for (int i = 0; i < block.rows; ++i) {
        for (int j = 0; j < block.cols; ++j) {
            int prediction = predictorClass.predict(pred, block, i, j);
            int error = block.at<uint8_t>(i, j) - prediction;
            predictBlock.at<uint8_t>(i, j) = static_cast<uint8_t>(error);
        }
    }

    // Use golomb to test the best frame type to encode and save the result
    std::vector<uint8_t> blockVector = Frame::mat_to_linear_vector(block);
    std::vector<uint8_t> predictBlockVector =
        Frame::mat_to_linear_vector(predictBlock);
    int bitsIntra = golomb.get_bits_needed(blockVector);
    int bitsPredict = golomb.get_bits_needed(predictBlockVector);
    if (bitsIntra < bitsPredict) {
        encodedBlock.type = I;
        encodedBlock.data = blockVector;
    } else {
        encodedBlock.type = P;
        encodedBlock.type = predictBlockVector;
    }

    return encodedBlock;
}

void GEncoder::encode_file_header(File file, size_t nBlocksPerFrame,
                                  int intraFramePeriodicity) {
    this->fileStruct = file;
    this->golomb.setApproach(fileStruct.approach);
    this->nBlocksPerFrame = nBlocksPerFrame;
    this->intraFramePeriodicity = intraFramePeriodicity;

    write_file_header();
}

void GEncoder::encode_frame(Frame frame, int frameId) {

    std::cout << "\nStarting processing frame " << std::setw(3) << frameId
              << " ..." << std::endl;

    // Divide frame in blocks
    std::vector<Mat> blocks = Frame::get_blocks(frame, fileStruct.blockSize);

    // Test/determine predictor
    PREDICTOR_TYPE pred = predictor;
    if (pred == AUTOMATIC) {
        pred = predictorClass.benchmark(frame);
    }

    // Use attributed m or calculate one
    int bM = m;
    if (bM < 1)
        bM = calculate_m(frame);

    // Determine frame type
    if (frameId % intraFramePeriodicity == 0)
        frame.type = I;
    else
        frame.type = P;

    // Write frame header
    FrameSegment segment;
    segment.type = frame.type;
    segment.m = bM;
    segment.predictor = pred;
    write_frame_header(segment);

    // Process and write to file each block
    int i = 1;
    for (Mat& block : blocks) {
        Block encodedBlock =
            process_block(block, i, nBlocksPerFrame, pred, segment);
        write_file_block(encodedBlock, i, nBlocksPerFrame);
        i++;
    }

    std::cout << "\nFrame processed, all data written to file\n" << endl;

    if (frameId == fileStruct.nFrames) {
        std::cout << "All frames encoded..." << std::endl;
    }
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
        fileStruct.type =
            static_cast<FILE_TYPE>(reader.readNBits(BITS_FILE_TYPE));
        fileStruct.blockSize = reader.readNBits(BITS_BLOCK_SIZE);
        fileStruct.nFrames = reader.readNBits(BITS_N_FRAMES);
        fileStruct.chroma = reader.readNBits(BITS_CHROMA);
        fileStruct.width = reader.readNBits(BITS_WIDTH);
        fileStruct.height = reader.readNBits(BITS_HEIGHT);
        fileStruct.fps = reader.readString(BITS_FPS);
        fileStruct.approach =
            static_cast<APPROACH>(reader.readNBits(BITS_APPROACH));
        fileStruct.lossy = reader.readNBits(BITS_LOSSY);
        headerRead = true;

        if (!check_approach(fileStruct.approach)) {
            cerr << "Error: Invalid approach type " << fileStruct.approach
                 << endl;
            exit(1);
        }

        golomb.setApproach(fileStruct.approach);
    }

    int numBlocksWidth = fileStruct.width / fileStruct.blockSize;
    int numBlocksHeight = fileStruct.height / fileStruct.blockSize;

    if (fileStruct.width % fileStruct.blockSize != 0)
        ++numBlocksWidth;  // Add another column of blocks not fully occupied

    if (fileStruct.height % fileStruct.blockSize != 0)
        ++numBlocksHeight;  // Add another row of blocks not fully occupied

    size_t nBlocksPerFrame =
        static_cast<size_t>(int(numBlocksWidth * numBlocksHeight));

    return nBlocksPerFrame;
}

FrameSegment GDecoder::read_frame_header() {
    FrameSegment segment;
    segment.type = static_cast<FRAME_TYPE>(reader.readNBits(BITS_FRAME_TYPE));
    segment.predictor = reader.readNBits(BITS_PREDICTOR);
    segment.m = reader.readNBits(BITS_M);
    golomb.setM(frame.m);
}

Block GDecoder::read_file_block(int blockId, int nBlocks, FrameSegment& frame) {
    Block block;

    // Read Frame type
    if (frame.type == I)
        block.type = I;
    else
        block.type = static_cast<FRAME_TYPE>(reader.readNBits(BITS_FRAME_TYPE));

    // Read Block data
    this->golomb.setM(frame.m);

    /*std::cout << " - Reading Block " << std::setw(3) << blockId << "/"
              << std::setw(3) << nBlocks << " with m = " << std::setw(3)
              << unsigned(block.m) << ", predictor = " << std::setw(1)
              << unsigned(block.predictor) << endl;*/

    // check m
    if (block.m < 1) {
        cerr << "Error: Encountered invalid m = " << unsigned(block.m) << endl;
        exit(2);
    }

    // Since edge blocks have different sizes than the regular ones, we need to test
    //  the edge cases.
    int nCol = fileStruct.blockSize;
    int nRows = fileStruct.blockSize;
    int numBlocksWidth = fileStruct.width / fileStruct.blockSize;
    int numBlocksHeight = fileStruct.height / fileStruct.blockSize;

    // If the blockId is a multiple of the number of blocks in a row and the final block
    //  in the row has a different size than the rest
    if ((blockId % numBlocksWidth) == 0 &&
        fileStruct.width % fileStruct.blockSize)
        nCols = fileStruct.width % fileStruct.blockSize;

    // If the blockId is a multiple of the number of blocks in a column and the final block
    //  in the column has a different size than the rest
    if ((blockId % numBlocksHeight) == 0 &&
        fileStruct.height % fileStruct.blockSize)
        nRows = fileStruct.height % fileStruct.blockSize;

    int nSamples = nCol * nRows;

    for (uint16_t i = 0; i < nSamples; i++) {
        int data = golomb.decode();
        block.data.push_back((uint8_t)data);
    }

    return block;
}

Mat GDecoder::decode_block(Mat& blockImg, FrameSegment& frame) {

    cv::Mat decodedBlock = cv::Mat::zeros(blockImg.size(), blockImg.type());
    for (int i = 0; i < blockImg.rows; ++i) {
        for (int j = 0; j < blockImg.cols; ++j) {
            int prediction =
                predictorClass.predict(frame.type, decodedBlock, i, j);
            int error = blockImg.at<uint8_t>(i, j) - prediction;
            decodedBlock.at<uint8_t>(i, j) = static_cast<uint8_t>(error);
        }
    }

    return decodedBlock;
}

Mat GDecoder::decode_frame(int frameId) {

    int nBlocksPerFrame = read_file_header();

    FrameSegment frameStruct = read_frame_header();

    std::vector<Mat> frameVector;
    for (int bId = 1; bId <= nBlocksPerFrame; bId++) {
        Block block = read_file_block(bId, nBlocksPerFrame, frameStruct);

        Mat blockImg = Frame::linear_vector_to_mat(block.data);

        if (block.type == I)
            frameVector.push_back(blockImg);
        else
            frameVector.push_back(decode_block(blockImg, frameStruct));
    }

    Mat frameImg = Frame::compose_blocks(frameVector, fileStruct.blockSize,
                                         fileStruct.height, fileStruct.width);

    return frameImg;
}