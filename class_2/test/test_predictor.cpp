// Import of standard or provided libraries
#include <gtest/gtest.h>
#include <cmath>
#include <string>

// Import of local libraries
#include <golomb_codec.h>

using namespace std;

TEST(Predictor, testPredictor1) {
    Predictor predictor;
    std::vector<short> a = {1};
    int res = predictor.predict(PREDICT1, a);
    EXPECT_EQ(res, 1);

    a = {};
    res = predictor.predict(PREDICT1, a);
    EXPECT_EQ(res, 0);
};

TEST(Predictor, testPredictor2) {
    Predictor predictor;
    std::vector<short> a = {1, 2};
    int res = predictor.predict(PREDICT2, a);
    EXPECT_EQ(res, 3);

    a = {};
    res = predictor.predict(PREDICT2, a);
    EXPECT_EQ(res, 0);

    a = {1};
    res = predictor.predict(PREDICT2, a);
    EXPECT_EQ(res, 2);
};

TEST(Predictor, testPredictor3) {
    Predictor predictor;
    std::vector<short> a = {1, 2, 3};
    int res = predictor.predict(PREDICT3, a);

    EXPECT_EQ(res, 4);

    a = {};
    res = predictor.predict(PREDICT3, a);
    EXPECT_EQ(res, 0);

    a = {1};
    res = predictor.predict(PREDICT3, a);
    EXPECT_EQ(res, 3);

    a = {1, 2};
    res = predictor.predict(PREDICT3, a);
    EXPECT_EQ(res, 3);
};

TEST(Predictor, testCheckType) {
    // Test if exit is launched
    Predictor predictor;
    PREDICTOR_TYPE invalidOption = static_cast<PREDICTOR_TYPE>(15);
    EXPECT_EQ(predictor.check_type(invalidOption), false);

    PREDICTOR_TYPE validOption = PREDICT1;
    EXPECT_EQ(predictor.check_type(validOption), true);
};

TEST(Predictor, testBadPredict) {
    // Test if exit is launched
    Predictor predictor;
    std::vector<short> a = {1, 2, 3};
    int invalidOption = 15;
    EXPECT_EXIT(
        predictor.predict(static_cast<PREDICTOR_TYPE>(invalidOption), a),
        ::testing::ExitedWithCode(2), "");
};

TEST(Predictor, testBenchmark) {
    Predictor predictor;

    std::vector<short> predict1_samples = {1, 3, 5, 7, 9};
    EXPECT_EQ(predictor.benchmark(predict1_samples), PREDICT1);

    std::vector<short> predict2_samples = {2, 4, 8, 16, 32};
    EXPECT_EQ(predictor.benchmark(predict2_samples), PREDICT2);

    std::vector<short> predict3_samples = {1, 2, 4, 7, 11, 16};
    EXPECT_EQ(predictor.benchmark(predict3_samples), PREDICT3);
};
