// Import of standard or provided libraries
#include <gtest/gtest.h>
#include <cmath>
#include <string>

// Import of local libraries
#include <bit_stream.h>

#define TEST_FILE_NAME "test_bitStream.bin"

using namespace std;

struct File {
    uint16_t blockSize;
    uint8_t nChannels;
    uint16_t sampleRate;
    uint32_t nFrames;
    uint16_t quantizationBits;
    bool lossy;  // true if lossy, false if lossless
    uint16_t timesPlayed;
};

TEST(BitStream, multipleValues) {
    File inF;
    inF.blockSize = 1024;
    inF.nChannels = 2;
    inF.sampleRate = 44100;
    inF.nFrames = 100;
    inF.quantizationBits = 8;
    inF.lossy = true;
    inF.timesPlayed = 12324;

    BitStream writer('w', TEST_FILE_NAME);

    writer.writeNBits(inF.blockSize, 16);
    writer.writeNBits(inF.nChannels, 8);
    writer.writeNBits(inF.sampleRate, 16);
    writer.writeNBits(inF.nFrames, 32);
    writer.writeNBits(inF.quantizationBits, 8);
    writer.writeNBits(inF.lossy, 2);
    writer.writeNBits(inF.timesPlayed, 16);

    writer.~BitStream();

    File outF;
    BitStream reader('r', TEST_FILE_NAME);

    outF.blockSize = reader.readNBits(16);
    outF.nChannels = reader.readNBits(8);
    outF.sampleRate = reader.readNBits(16);
    outF.nFrames = reader.readNBits(32);
    outF.quantizationBits = reader.readNBits(8);
    outF.lossy = reader.readNBits(2);
    outF.timesPlayed = reader.readNBits(16);

    reader.~BitStream();

    EXPECT_EQ(inF.blockSize, outF.blockSize);
    EXPECT_EQ(inF.nChannels, outF.nChannels);
    EXPECT_EQ(inF.sampleRate, outF.sampleRate);
    EXPECT_EQ(inF.nFrames, outF.nFrames);
    EXPECT_EQ(inF.quantizationBits, outF.quantizationBits);
    EXPECT_EQ(inF.lossy, outF.lossy);
    EXPECT_EQ(inF.timesPlayed, outF.timesPlayed);

    remove(TEST_FILE_NAME);
};