#ifndef ANIMEPREDICTOR_MODEL_H
#define ANIMEPREDICTOR_MODEL_H

#include "tensorflow/lite/interpreter.h"
#include "tensorflow/lite/kernels/register.h"
#include "tensorflow/lite/tools/gen_op_registration.h"

#include <nlohmann/json.hpp>

struct AnimeEntry {
    std::string name;
    std::string type;
    int episodes;
    float rating;
    int members;
    std::vector<std::string> genres;
};

class Model {
private:
    nlohmann::json _metadata;

    std::vector<std::string> _featureColumns;
    std::unique_ptr<tflite::FlatBufferModel> _model;
    tflite::ops::builtin::BuiltinOpResolver _resolver;
    std::unique_ptr<tflite::Interpreter> _interpreter;

    std::vector<AnimeEntry> all_anime;

public:
    void LoadMetadata(const std::string& path);
    void LoadModel(const std::string& path);
    void LoadDataBase(const std::string& path);

    void PredictOnDatabase();
    void PredictOnUI();

private:
    std::vector<float> BuildInputVector(const AnimeEntry& anime,
                                        const nlohmann::json& metadata,
                                        const std::vector<std::string>& feature_columns);

    float predict_rating(tflite::Interpreter* interpreter, const std::vector<float>& features);
};

#endif //ANIMEPREDICTOR_MODEL_H
