#pragma once

#include <golomb.h>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>

using namespace std;

/*
##############################################################################
###################           Predictor Class              ###################
##############################################################################
*/

enum PREDICTOR_TYPE { AUTOMATIC, PREDICT1, PREDICT2, PREDICT3 };

std::string get_type_string(PREDICTOR_TYPE type);

class Predictor {
   private:
    /*! a1, a2 and a3 represent, a(n-1) a(n-2) and a(n-3) respectively
            where n is the index of the predicted sample */
    int predict1(int a1);
    int predict2(int a1, int a2);
    int predict3(int a1, int a2, int a3);

    double calculate_entropy(PREDICTOR_TYPE type, std::vector<short>& samples);

   public:
    Predictor();
    ~Predictor();

    /*! 
        Pass a set of samples/block and return the best predictor to be used 
            (the one that resulted in less occupied space)
    */
    PREDICTOR_TYPE benchmark(std::vector<short>& samples);

    /*!
        Predict the next sample based on the type of the predictor and the
            previous samples
    */
    int predict(PREDICTOR_TYPE type, std::vector<short>& samples, int index);

    bool check_type(PREDICTOR_TYPE type);
};