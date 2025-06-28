#include "ImGUI_Training.h"
#include "ImGUI/ImGUI_Manager.h"

#include "Parser/Parser.h"

#include "Logger/LogManager.h"

bool ImGUI_Training::Render() {
    if (ImGui::BeginTabItem("Обучение")) {
        ImGui::Text("Интерфейс для обучения модели");
        ImGui::Text("Внимание: для обучения модели на своих данных у Вас должен быть установлен Docker");
        ImGui::Separator();

        animePath.reserve(256);
        ratingPath.reserve(256);

        InputTextStdString("Путь к anime.csv", animePath);
        InputTextStdString("Путь к rating.csv", ratingPath);

        TrainButton();
        LogMenu();

        ImGui::EndTabItem();
        return true;
    }
    return false;
}

void ImGUI_Training::TrainButton() {
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
}

void ImGUI_Training::LogMenu() {
    static std::string bufferName = "Training";
    LogManager& log = LogManager::GetInstance();

    // Буфер, необходимый для отображения
    static size_t messageCount = 0;
    static std::string buffer;
    static size_t previousLogSize = 0;

    size_t counter = 0;
    for(const auto& it : log.GetCustomLogBuffer(bufferName)) {
        if (messageCount == log.GetCustomLogBuffer(bufferName).size()) { break; }
        if (counter < messageCount) { counter++; continue; }

        //Remove format

        buffer += it + "\n";
        counter++;
        messageCount = counter;
    }

    // Начинаем Child-зону для скроллинга
    ImGui::BeginChild("LogScrollRegion", ImVec2(-1, -1), true, ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::TextUnformatted(buffer.c_str());

    // Автоскролл, если добавлены новые данные
    if (buffer.size() > previousLogSize) {
        ImGui::SetScrollHereY(1.0f);  // Прокрутка к низу
        previousLogSize = buffer.size();
    }

    ImGui::EndChild();
}

void ImGUI_Training::TrainModel(const std::string& animePath, const std::string& ratingPath) {
    auto instance = Parser::GetInstance();

    instance.ImportTables(animePath, ratingPath);
    instance.ExportTables("Training/out/anime.csv", "Training/out/rating.csv");
}

void ImGUI_Training::RunDocker() {
    static std::string bufferName = "Training";
    // Проверка Docker-образа
    std::string imageName = "anime-trainer";
    std::string containerName = "anime-trainer-container";

    LogManager::LogCustom(false, bufferName,"[Docker] Checking for image availability " + imageName + "...");
    std::string searchCommand = "docker image inspect " + imageName + " > /dev/null 2>&1";
    int checkResult = system(searchCommand.c_str());

    // Сборка образа при необходимости
    if (checkResult != 0) {
        LogManager::LogCustom(false, bufferName, "[Docker] Image not found. Building from Dockerfile...");
        std::string buildCommand = "docker build -t " + imageName + " -f Training/Dockerfile Training";
        int buildResult = system(buildCommand.c_str());
        if (buildResult != 0) {
            LogManager::LogCustom(false, bufferName, "Error while building Docker image");
            return;
        }
    } else {
        LogManager::LogCustom(false, bufferName, "[Docker] Image found");
    }

    // Подготовка путей
    std::filesystem::path dockerDataPath = std::filesystem::absolute("Training/out");
    std::filesystem::path dockerModelExportPath = std::filesystem::absolute("Training/model_export");

    // Создание директорий если не существует
    std::filesystem::create_directories(dockerDataPath);
    std::filesystem::create_directories(dockerModelExportPath);

    // Запуск контейнера
    LogManager::LogCustom(false, bufferName, "[Docker] Running a container...");
    std::string runCommand = "docker run -it --name " + containerName + " " + imageName;
    int result = system(runCommand.c_str());

    if (result != 0) {
        LogManager::LogCustom(false, bufferName, "Container training failed with error " + __LOGERROR__);
    } else {
        std::string copyCommand = "docker cp " + containerName + ":/app/model_export/. ./Training/model_export";
        system(copyCommand.c_str());

        LogManager::LogCustom(false, bufferName, "Training completed. Model files are available in Training/model_export");
    }

    std::string removeCommand = "docker rm " + containerName;
    system(removeCommand.c_str());
}