// Import of standard or provided libraries
#include <gtest/gtest.h>
#include <cmath>
#include <string>

// Import of local libraries
#include <golomb_codec.h>

using namespace std;

TEST(Predictor, predictor1) {
    Predictor predictor;
    std::vector<short> a = {1};
    int res = predictor.predict(PREDICT1, a);
    EXPECT_EQ(res, 1);

    a = {};
    res = predictor.predict(PREDICT1, a);
    EXPECT_EQ(res, 0);
};

TEST(Predictor, predictor2) {
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

TEST(Predictor, predictor3) {
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

TEST(Predictor, checkType) {
    // Test if exit is launched
    Predictor predictor;
    PREDICTOR_TYPE invalidOption = static_cast<PREDICTOR_TYPE>(15);
    EXPECT_EQ(predictor.check_type(invalidOption), false);

    PREDICTOR_TYPE validOption = PREDICT1;
    EXPECT_EQ(predictor.check_type(validOption), true);
};

TEST(Predictor, badPredict) {
    // Test if exit is launched
    Predictor predictor;
    std::vector<short> a = {1, 2, 3};
    int invalidOption = 15;
    EXPECT_EXIT(
        predictor.predict(static_cast<PREDICTOR_TYPE>(invalidOption), a),
        ::testing::ExitedWithCode(2), "");
};

TEST(Predictor, benchmark){

};
