#include <image_predictor.h>

/*
##############################################################################
###################           Predictor Class              ###################
##############################################################################
*/

std::string get_type_string(PREDICTOR_TYPE type) {
    std::string predText = "AUTOMATIC (0)";
    if (type == PREDICT1)
        predText = "PREDICT1 (1)";
    else if (type == PREDICT2)
        predText = "PREDICT2 (2)";
    else if (type == PREDICT3)
        predText = "PREDICT3 (3)";
    else
        predText = "UNKNOWN";
    return predText;
}

Predictor::Predictor() {}

Predictor::~Predictor() {}

int Predictor::predict1(int a1) {
    return a1;
}

int Predictor::predict2(int a1, int a2) {
    return (2 * a1) - a2;
}

int Predictor::predict3(int a1, int a2, int a3) {
    return (3 * a1) - (3 * a2) + a3;
}

double Predictor::calculate_entropy(PREDICTOR_TYPE type,
                                    std::vector<short>& samples) {
    int total_predictions = 0;
    std::vector<int> predictions;

    // Predict based on the given type and count occurrences of predictions
    for (size_t i = 0; i < samples.size(); ++i) {
        int prediction;
        if (type == PREDICT1) {
            prediction = predict(type, samples, i);
        } else if (type == PREDICT2) {
            prediction = predict(type, samples, i);
        } else if (type == PREDICT3) {
            prediction = predict(type, samples, i);
        } else {
            cerr << "Error: Unknown Predictor type encountered" << endl;
            exit(2);
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

int Predictor::predict(PREDICTOR_TYPE type, std::vector<short>& samples,
                       int index) {
    if (!check_type(type) || type == AUTOMATIC) {
        cerr << "Error: Unknown/Invalid Predictor " << unsigned(type)
             << " encountered while decoding Block" << endl;
        exit(2);
    }

    int a1, a2, a3;

    a1 = (index - 1) < 0 ? 0 : samples.at(index - 1);
    a2 = (index - 2) < 0 ? 0 : samples.at(index - 2);
    a3 = (index - 3) < 0 ? 0 : samples.at(index - 3);

    if (type == PREDICT1)
        return predict1(a1);
    else if (type == PREDICT2)
        return predict2(a1, a2);
    else
        return predict3(a1, a2, a3);
}

bool Predictor::check_type(PREDICTOR_TYPE type) {
    if (type == AUTOMATIC || type == PREDICT1 || type == PREDICT2 ||
        type == PREDICT3)
        return true;
    return false;
}

PREDICTOR_TYPE Predictor::benchmark(std::vector<short>& samples) {
    double min_entropy = std::numeric_limits<double>::max();
    PREDICTOR_TYPE best_predictor = AUTOMATIC;

    double entropy1 = calculate_entropy(PREDICT1, samples);
    if (entropy1 < min_entropy) {
        min_entropy = entropy1;
        best_predictor = PREDICT1;
    }

    double entropy2 = calculate_entropy(PREDICT2, samples);
    if (entropy2 < min_entropy) {
        min_entropy = entropy2;
        best_predictor = PREDICT2;
    }

    double entropy3 = calculate_entropy(PREDICT3, samples);
    if (entropy3 < min_entropy) {
        min_entropy = entropy3;
        best_predictor = PREDICT3;
    }

    return best_predictor;
}