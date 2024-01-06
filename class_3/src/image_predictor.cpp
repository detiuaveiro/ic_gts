#include <image_predictor.h>

/*
##############################################################################
###################           Predictor Class              ###################
##############################################################################
*/

Predictor::Predictor() {}

Predictor::~Predictor() {}

std::string get_type_string(PREDICTOR_TYPE type) {
    std::string predText = "AUTOMATIC (0)";
    if (type == JPEG1)
        predText = "JPEG1 (1)";
    else if (type == JPEG2)
        predText = "JPEG2 (2)";
    else if (type == JPEG3)
        predText = "JPEG3 (3)";
    else if (type == JPEG4)
        predText = "JPEG4 (4)";
    else if (type == JPEG5)
        predText = "JPEG5 (5)";
    else if (type == JPEG6)
        predText = "JPEG6 (6)";
    else if (type == JPEG7)
        predText = "JPEG7 (7)";
    else if (type == JPEG_LS)
        predText = "JPEG_LS (8)";
    else
        predText = "UNKNOWN";
    return predText;
}

bool Predictor::check_type(PREDICTOR_TYPE type) {
    if (type >= 0 && type <= 8)
        return true;
    return false;
}

int Predictor::predict_jpeg_1(int a) {
    return a;
}

int Predictor::predict_jpeg_2(int b) {
    return b;
}

int Predictor::predict_jpeg_3(int c) {
    return c;
}

int Predictor::predict_jpeg_4(int a, int b, int c) {
    return a + b - c;
}

int Predictor::predict_jpeg_5(int a, int b, int c) {
    return a + (b - c) / 2;
}

int Predictor::predict_jpeg_6(int a, int b, int c) {
    return b + (a - c) / 2;
}

int Predictor::predict_jpeg_7(int a, int b) {
    return (a + b) / 2;
}

int Predictor::predict_jpeg_LS(int a, int b, int c) {
    if (c >= max(a, b))
        return min(a, b);
    else if (c <= min(a, b))
        return max(a, b);
    else
        return a + b - c;
}

double Predictor::calculate_entropy(PREDICTOR_TYPE type,
                                    std::vector<vector<uint8_t>>& samples) {

    if (!check_type(type) || type == AUTOMATIC) {
        cerr << "Error: Unknown/Invalid Predictor " << unsigned(type)
             << " encountered while calculating entropy" << endl;
        exit(2);
    }

    int total_predictions = 0;
    std::vector<int> predictions;

    // Predict based on the given type and count occurrences of predictions
    for (size_t i = 0; i < samples.size(); ++i) {
        int prediction = predict(type, samples, i, 1);
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

int Predictor::predict(PREDICTOR_TYPE type, vector<vector<uint8_t>>& block,
                       int idX, int idY) {
    if (!check_type(type) || type == AUTOMATIC) {
        cerr << "Error: Unknown/Invalid Predictor " << unsigned(type)
             << " encountered while decoding Block" << endl;
        exit(2);
    }

    // change this to calculate correctly
    int a = idX - idY;
    int b = idX - idY;
    int c = idX - idY;

    if (type == JPEG1)
        return predict_jpeg_1(a);
    else if (type == JPEG2)
        return predict_jpeg_2(b);
    else if (type == JPEG3)
        return predict_jpeg_3(c);
    else if (type == JPEG4)
        return predict_jpeg_4(a, b, c);
    else if (type == JPEG5)
        return predict_jpeg_5(a, b, c);
    else if (type == JPEG6)
        return predict_jpeg_6(a, b, c);
    else if (type == JPEG7)
        return predict_jpeg_7(a, b);
    else
        return predict_jpeg_LS(a, b, c);
}

PREDICTOR_TYPE Predictor::benchmark(std::vector<vector<uint8_t>>& samples) {
    double min_entropy = std::numeric_limits<double>::max();
    PREDICTOR_TYPE best_predictor = AUTOMATIC;

    std::array<double, 8> entropies = {
        calculate_entropy(JPEG1, samples), calculate_entropy(JPEG2, samples),
        calculate_entropy(JPEG3, samples), calculate_entropy(JPEG4, samples),
        calculate_entropy(JPEG5, samples), calculate_entropy(JPEG6, samples),
        calculate_entropy(JPEG7, samples), calculate_entropy(JPEG_LS, samples)};

    for (size_t i = 0; i < entropies.size(); ++i) {
        if (entropies[i] < min_entropy) {
            min_entropy = entropies[i];
            best_predictor = static_cast<PREDICTOR_TYPE>(i);
        }
    }

    return best_predictor;
}
