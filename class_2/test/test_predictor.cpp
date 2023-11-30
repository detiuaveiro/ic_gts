// Import of standard or provided libraries
#include <gtest/gtest.h>
#include <cmath>
#include <string>
#include <tuple>

// Import of local libraries
#include <golomb_codec.h>

using namespace std;

TEST(Predictor, testPredictor1) {
    Predictor predictor;
    std::vector<short> a = {1};
    int res = predictor.predict(PREDICT1, NO_CORRELATION, a, 1);
    EXPECT_EQ(res, 1);

    a = {};
    res = predictor.predict(PREDICT1, NO_CORRELATION, a, 0);
    EXPECT_EQ(res, 0);
};

TEST(Predictor, testPredictor2) {
    Predictor predictor;
    std::vector<short> a = {1, 2};
    int res = predictor.predict(PREDICT2, NO_CORRELATION, a, 2);
    EXPECT_EQ(res, 3);

    a = {};
    res = predictor.predict(PREDICT2, NO_CORRELATION, a, 0);
    EXPECT_EQ(res, 0);

    a = {1};
    res = predictor.predict(PREDICT2, NO_CORRELATION, a, 1);
    EXPECT_EQ(res, 2);
};

TEST(Predictor, testPredictor3) {
    Predictor predictor;
    std::vector<short> a = {1, 2, 3};
    int res = predictor.predict(PREDICT3, NO_CORRELATION, a, 3);

    EXPECT_EQ(res, 4);

    a = {};
    res = predictor.predict(PREDICT3, NO_CORRELATION, a, 0);
    EXPECT_EQ(res, 0);

    a = {1};
    res = predictor.predict(PREDICT3, NO_CORRELATION, a, 1);
    EXPECT_EQ(res, 3);

    a = {1, 2};
    res = predictor.predict(PREDICT3, NO_CORRELATION, a, 2);
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
    int invalidPredictor = 15;
    int invalidPhase = 5;
    EXPECT_EXIT(predictor.predict(static_cast<PREDICTOR_TYPE>(invalidPredictor),
                                  NO_CORRELATION, a, 3),
                ::testing::ExitedWithCode(2), "");
    EXPECT_EXIT(
        predictor.predict(PREDICT1, static_cast<PHASE>(invalidPhase), a, 3),
        ::testing::ExitedWithCode(2), "");
};

TEST(Predictor, testBenchmark) {
    Predictor predictor1(1);

    std::vector<short> predict1_samples = {1, 1, 1, 1, 1};
    std::tuple<int, int> ret1 = predictor1.benchmark(predict1_samples);
    EXPECT_EQ(static_cast<PREDICTOR_TYPE>(std::get<0>(ret1)), PREDICT1);

    std::vector<short> predict2_samples = {2, 4, 8, 16, 32};
    std::tuple<int, int> ret2 = predictor1.benchmark(predict2_samples);
    EXPECT_EQ(static_cast<PREDICTOR_TYPE>(std::get<0>(ret2)), PREDICT2);

    std::vector<short> predict3_samples = {1, 5, 14, 30, 55};
    std::tuple<int, int> ret3 = predictor1.benchmark(predict3_samples);
    EXPECT_EQ(static_cast<PREDICTOR_TYPE>(std::get<0>(ret3)), PREDICT3);
}
