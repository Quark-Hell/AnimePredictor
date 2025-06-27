#include "Loader.h"
#include "Logger/LogManager.h"

#include <iostream>

void Loader::Test() {
    Model predictor;

    try {
        // Загрузка метаданных и модели
        predictor.LoadMetadata("Training/model_export/metadata.json");
        predictor.LoadModel("Training/model_export/model.tflite");

        // Загрузка аниме-базы
        predictor.LoadDataBase("Training/out/anime.csv");

        // Перемешиваем и берём 10 случайных
        predictor.PredictOnDatabase();
        //predictor.PredictOnUI();

    } catch (const std::exception& ex) {
        LogManager::LogCritical(std::string("Ошибка: ") + ex.what() + " " + __LOGERROR__);
    }
}