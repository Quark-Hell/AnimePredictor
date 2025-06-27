#include "Model.h"

#include "Logger/LogManager.h"

#include <fstream>
#include <vector>
#include <map>
#include <string>
#include <random>
#include <algorithm>

void Model::LoadMetadata(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) {
        LogManager::LogCritical("metadata.json not found " + __LOGERROR__);
    }
    in >> _metadata;
}

void Model::LoadModel(const std::string& path) {
    _featureColumns = _metadata["feature_columns"].get<std::vector<std::string>>();

    _model = tflite::FlatBufferModel::BuildFromFile(path.c_str());
    tflite::InterpreterBuilder(*_model, _resolver)(&_interpreter);
    _interpreter->AllocateTensors();
}

void Model::LoadDataBase(const std::string& path) {
    all_anime.clear();

    std::ifstream in(path);
    if (!in.is_open()){
        LogManager::LogCritical("anime.csv not found " + __LOGERROR__);
    }

    std::string line;
    std::getline(in, line); // skip header

    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string token;
        std::vector<std::string> fields;
        bool inside_quotes = false;
        std::string field;

        for (char ch : line) {
            if (ch == '"') {
                inside_quotes = !inside_quotes;
            } else if (ch == ',' && !inside_quotes) {
                fields.push_back(field);
                field.clear();
            } else {
                field += ch;
            }
        }
        fields.push_back(field);

        if (fields.size() < 7) continue;

        AnimeEntry entry;
        entry.name = fields[1];
        entry.genres = {};
        std::stringstream genre_ss(fields[2]);
        std::string genre;
        while (std::getline(genre_ss, genre, '|')) {
            entry.genres.push_back(genre);
        }
        entry.type = fields[3];
        entry.episodes = std::stoi(fields[4]);
        entry.rating = std::stof(fields[5]);
        entry.members = std::stoi(fields[6]);
        all_anime.push_back(entry);
    }
}

void Model::PredictOnDatabase() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(all_anime.begin(), all_anime.end(), g);

    LogManager::LogInfo("🎲 Random 10 Anime Predictions:");
    LogManager::LogInfo("---------------------------------------------");

    for (int i = 0; i < 10 && i < all_anime.size(); ++i) {
        const auto& anime = all_anime[i];
        auto input = BuildInputVector(anime, _metadata, _featureColumns);
        float prediction = predict_rating(_interpreter.get(), input);

        LogManager::LogInfo("[" + std::to_string(i + 1) + "] " + anime.name);
        LogManager::LogInfo("   True Rating      : " + std::to_string(anime.rating));
        LogManager::LogInfo("   Predicted Rating : " + std::to_string(prediction));
        LogManager::LogInfo("---------------------------------------------");
    }
}

void Model::PredictOnUI() {
    LogManager::LogInfo("Введите данные для предсказания рейтинга нового аниме");
    AnimeEntry custom;
    std::string line;

    std::cout << "Название аниме: ";
    std::getline(std::cin, custom.name);

    std::cout << "Тип аниме (например, TV, Movie, OVA): ";
    std::getline(std::cin, custom.type);

    std::cout << "Количество эпизодов: ";
    std::getline(std::cin, line);
    custom.episodes = line.empty() ? 1 : std::stoi(line);

    std::cout << "Жанры (через запятую, например Action,Romance): ";
    std::getline(std::cin, line);
    std::stringstream genre_stream(line);
    std::string genre;
    while (std::getline(genre_stream, genre, ',')) {
        // Удаление лишних пробелов
        genre.erase(0, genre.find_first_not_of(" \t"));
        genre.erase(genre.find_last_not_of(" \t") + 1);
        if (!genre.empty()) {
            custom.genres.push_back(genre);
        }
    }

    // Поскольку модель не использует эти поля, задаём значения по умолчанию:
    custom.rating = 10.0f;
    custom.members = 0;

    auto input = BuildInputVector(custom, _metadata, _featureColumns);
    float prediction = predict_rating(_interpreter.get(), input);

    std::cout << "\n📈 Предсказанный рейтинг для \"" << custom.name << "\" = " << prediction << "\n";
}

std::vector<float> Model::BuildInputVector(const AnimeEntry& anime,
                                           const nlohmann::json& metadata,
                                           const std::vector<std::string>& feature_columns) {
    std::vector<float> features;

    auto genre_classes = metadata["genre_classes"].get<std::vector<std::string>>();
    auto type_classes = metadata["type_classes"].get<std::vector<std::string>>();
    auto mean = metadata["scaler_mean"].get<std::vector<float>>();
    auto scale = metadata["scaler_scale"].get<std::vector<float>>();

    std::map<std::string, float> raw_feature_values = {
            {"type_encoded", 0},
            {"episodes", static_cast<float>(anime.episodes)},
            {"user_avg_rating", 7.0f},
            {"user_rating_std", 1.0f},
            {"user_rating_count", 30.0f},
            {"anime_user_rating_std", 1.0f},
            {"anime_user_rating_count", 100.0f},
    };


    // Кодирование типа
    for (size_t i = 0; i < type_classes.size(); ++i) {
        if (anime.type == type_classes[i]) {
            raw_feature_values["type_encoded"] = static_cast<float>(i);
            break;
        }
    }

    // Формирование вектора признаков
    for (const auto& col : feature_columns) {
        if (col.rfind("genre_", 0) == 0) {
            std::string genre = col.substr(6);
            auto it = std::find(anime.genres.begin(), anime.genres.end(), genre);
            features.push_back(it != anime.genres.end() ? 1.0f : 0.0f);
        } else {
            auto it = raw_feature_values.find(col);
            features.push_back(it != raw_feature_values.end() ? it->second : 0.0f);
        }
    }

    // Масштабирование (StandardScaler)
    for (size_t i = 0; i < features.size(); ++i) {
        features[i] = (features[i] - mean[i]) / scale[i];
    }

    return features;
}


float Model::predict_rating(tflite::Interpreter* interpreter, const std::vector<float>& features) {
    int input_idx = interpreter->inputs()[0];
    float* input_tensor = interpreter->typed_input_tensor<float>(input_idx);
    std::copy(features.begin(), features.end(), input_tensor);

    if (interpreter->Invoke() != kTfLiteOk) {
        LogManager::LogError("Model inference failed " + __LOGERROR__);
    }

    float* output = interpreter->typed_output_tensor<float>(0);
    return output[0];
}