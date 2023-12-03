// Import of standard or provided libraries
#include <gtest/gtest.h>
#include <cmath>
#include <string>

// Import of local libraries
#include <golomb_codec.h>

#define TEST_FILE_NAME "test_golomb_encoder.bin"

using namespace std;

TEST(GEncoder, testCalculateM) {
    GEncoder gEncoder(TEST_FILE_NAME, 5, AUTOMATIC, NO_CORRELATION);

    std::vector<short> samples = {1, 2, 3, 4, 5};
    int m = gEncoder.test_calculate_m(samples);

    EXPECT_EQ(m, 3);
};

TEST(GEncoder, testAbsValues) {
    GEncoder gEncoder(TEST_FILE_NAME, 5, AUTOMATIC, NO_CORRELATION);

    std::vector<short> samples = {-1, 2, -3, 4, -5};
    std::vector<unsigned short> abs_values =
        gEncoder.test_abs_value_vector(samples);

    EXPECT_EQ(samples.size(), abs_values.size());

    for (int i = 0; i < (int)samples.size(); i++) {
        EXPECT_EQ(abs(samples[i]), abs_values[i]);
    }
};

TEST(GEncoder, test_lossy_error_same_bitRate) {

    File inF;
    inF.blockSize = 1024;
    inF.nChannels = 1;
    inF.sampleRate = 44100;
    inF.nFrames = 529200;
    inF.bitRate = 705;  // CURRENT is 705 kbps
    inF.lossy = true;
    inF.approach = SIGN_MAGNITUDE;

    PREDICTOR_TYPE pred = PREDICT1;
    PHASE phase = NO_CORRELATION;

    GEncoder gEncoder(TEST_FILE_NAME, 5, pred, phase);
    gEncoder.setFile(inF);

    Predictor predictorClass;

    std::vector<short> samples = {7, 7, 8, 8, 9, 10};

    for (int i = 0; i < (int)samples.size(); i++) {
        int prediction = predictorClass.predict(pred, phase, samples, i);
        int error = samples.at(i) - prediction;
        int quantizedError =
            gEncoder.test_lossy_error(error, pred, phase, i, samples);

        // same bit rate, data shouldn't be affected
        EXPECT_EQ(error, quantizedError);
    }
};


TEST(GEncoder, test_lossy_error_smaller_bitRate) {

    File inF;
    inF.blockSize = 1024;
    inF.nChannels = 1;
    inF.sampleRate = 44100;
    inF.nFrames = 529200;
    inF.bitRate = 300;  // CURRENT is 705 kbps
    inF.lossy = true;
    inF.approach = SIGN_MAGNITUDE;

    PREDICTOR_TYPE pred = PREDICT1;
    PHASE phase = NO_CORRELATION;

    GEncoder gEncoder(TEST_FILE_NAME, 5, pred, phase);
    gEncoder.setFile(inF);

    Predictor predictorClass;

    std::vector<short> samples = {7, 7, 8, 8, 9, 10};
    std::vector<short> rawError = {7, 0, 1, 0, 1, 1};
    std::vector<short> quantizedError = {7, 0, 0, 0, 1, 0};
    std::vector<short> quantizedSamples = {7, 7, 7, 7, 9, 10};
    std::vector<short> calculatedError;
    std::vector<short> calculatedSamples;

    for (int i = 0; i < (int)samples.size(); i++) {
        int prediction = predictorClass.predict(pred, phase, samples, i);
        int error = samples.at(i) - prediction;
        int adjustedError =
            gEncoder.test_lossy_error(error, pred, phase, i, samples);

        cout << "\nerror: " << error << endl;

        // different bitRate, data should be altered
        //EXPECT_EQ(error, rawError.at(i));
        calculatedError.push_back(adjustedError);

        int sample =
            adjustedError + predictorClass.predict(pred, phase, samples, i);
        calculatedSamples.push_back(sample);
    }

    cout << "calculated error: ";
    for (int i = 0; i < (int)calculatedError.size(); i++) {
        cout << calculatedError.at(i) << " ";
        //EXPECT_EQ(calculatedError.at(i), quantizedError.at(i));
    }
    cout << endl;

    cout << "calculated samples: ";
    for (int i = 0; i < (int)calculatedSamples.size(); i++) {
        cout << calculatedSamples.at(i) << " ";
        //EXPECT_EQ(calculatedSamples.at(i), quantizedSamples.at(i));
    }
    cout << endl;

    cout << "samples: ";
    for (int i = 0; i < (int)samples.size(); i++) {
        cout << samples.at(i) << " ";
    }
    cout << endl;

    /*EXPECT_EQ(adjustedError, quantizedError.at(i));

    int sample =
        adjustedError + predictorClass.predict(pred, phase, samples, i);
    EXPECT_EQ(sample, quantizedSamples.at(i));*/
};
