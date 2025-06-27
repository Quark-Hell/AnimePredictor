#include "Loader.h"

#include <iostream>

void Loader::Test() {
    Model predictor;

    try {
        // Загрузка метаданных и модели
        predictor.LoadMetadata("model_export/metadata.json");
        predictor.LoadModel("model_export/model.tflite");

        // Загрузка аниме-базы
        predictor.LoadDataBase("out/anime.csv");

        // Перемешиваем и берём 10 случайных
        predictor.PredictOnDatabase();
        predictor.PredictOnUI();

    } catch (const std::exception& ex) {
        std::cerr << "Ошибка: " << ex.what() << "\n";
    }
}