#include "ImGUI_StartTraining.h"

#include "Parser/Parser.h"
#include "Logger/LogManager.h"

bool ImGUI_StartTraining::Render() {
    if (!trainingInProgress && ImGui::Button("Обучить модель")) {
        trainingInProgress = true;
        trainingCompleted = false;

        trainingThread = std::thread([=, this]() {
            TrainModel(animePath, ratingPath);
            RunDocker();

            trainingCompleted = true;
            trainingInProgress = false;
        });
        trainingThread.detach();
    }

    if (trainingInProgress) {
        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Обучение модели запущено...");
        ImGui::SameLine();
        ImGui::Text("(это может занять до часа)");
    } else if (trainingCompleted) {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Обучение завершено!");
    }

    return true;
}


void ImGUI_StartTraining::TrainModel(const std::string& animePath, const std::string& ratingPath) {
    auto instance = Parser::GetInstance();

    instance.ImportTables(animePath, ratingPath);
    instance.ExportTables("Training/out/anime.csv", "Training/out/rating.csv");
}

void ImGUI_StartTraining::RunDocker() {
    // Проверка Docker-образа
    std::string imageName = "anime-trainer";
    std::string containerName = "anime-trainer-container";

    LogManager::LogInfo("[Docker] Проверка наличия образа " + imageName + "...");
    std::string searchCommand = "docker image inspect " + imageName + " > /dev/null 2>&1";
    int checkResult = system(searchCommand.c_str());

    // Сборка образа при необходимости
    if (checkResult != 0) {
        LogManager::LogWarning("[Docker] Image not found. Building from Dockerfile...");
        std::string buildCommand = "docker build -t " + imageName + " -f Training/Dockerfile Training";
        int buildResult = system(buildCommand.c_str());
        if (buildResult != 0) {
            LogManager::LogError("Error while building Docker image");
            return;
        }
    } else {
        LogManager::LogInfo("[Docker] Image found");
    }

    // Подготовка путей
    std::filesystem::path dockerDataPath = std::filesystem::absolute("Training/out");
    std::filesystem::path dockerModelExportPath = std::filesystem::absolute("Training/model_export");

    // Создание директорий если не существует
    std::filesystem::create_directories(dockerDataPath);
    std::filesystem::create_directories(dockerModelExportPath);

    // Запуск контейнера
    LogManager::LogInfo("[Docker] Running a container...");
    std::string runCommand = "docker run -it --name " + containerName + " " + imageName;
    int result = system(runCommand.c_str());

    if (result != 0) {
        LogManager::LogError("Container training failed with error " + __LOGERROR__);
    } else {
        std::string copyCommand = "docker cp " + containerName + ":/app/model_export/. ./Training/model_export";
        system(copyCommand.c_str());

        LogManager::LogInfo("Training completed. Model files are available in Training/model_export");
    }

    std::string removeCommand = "docker rm " + containerName;
    system(removeCommand.c_str());
}