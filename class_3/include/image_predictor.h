#pragma once

#include <frame.h>
#include <golomb.h>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>

using namespace std;

enum PREDICTOR_TYPE {
    AUTOMATIC,
    JPEG1,
    JPEG2,
    JPEG3,
    JPEG4,
    JPEG5,
    JPEG6,
    JPEG7,
    JPEG_LS  // JPEG-LS is a non-linear predictor
};

/*
##############################################################################
###################           Predictor Class              ###################
##############################################################################
*/

std::string get_type_string(PREDICTOR_TYPE type);

class Predictor {
   private:
    int predict_jpeg_1(int a);
    int predict_jpeg_2(int b);
    int predict_jpeg_3(int c);
    int predict_jpeg_4(int a, int b, int c);
    int predict_jpeg_5(int a, int b, int c);
    int predict_jpeg_6(int a, int b, int c);
    int predict_jpeg_7(int a, int b);
    int predict_jpeg_LS(int a, int b, int c);

    double calculate_entropy(PREDICTOR_TYPE type, Mat& frame);

   public:
    Predictor();
    ~Predictor();

    /*! 
        Pass a set of frames/block and return the best predictor to be used 
            (the one that resulted in less occupied space)
    */
    PREDICTOR_TYPE benchmark(Mat& frame);

    /*!
        Predict the next sample based on the type of the predictor and the
            previous samples
    */
    int predict(PREDICTOR_TYPE type, Mat& frame, int idX, int idY);

    bool check_type(PREDICTOR_TYPE type);
};