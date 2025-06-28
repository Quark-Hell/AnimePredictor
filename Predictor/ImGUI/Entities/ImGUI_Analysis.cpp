#include "ImGUI_Analysis.h"

#include "Logger/LogManager.h"

bool ImGUI_Analysis::Render() {
    if (ImGui::BeginTabItem("Анализ")) {
        ImGui::Text("Интерфейс для обучения модели");
        ImGui::Separator();

        ImGui::Text("Панель предсказания рейтинга аниме:");
        static std::string animeName;
        static std::string animeType;
        static std::string animeEpisodes;
        static std::string animeGenres;

        animeName.reserve(128);
        animeType.reserve(64);
        animeEpisodes.reserve(16);
        animeGenres.reserve(256);

        InputTextStdString("Название аниме", animeName);
        InputTextStdString("Тип аниме", animeType);
        InputTextStdString("Количество эпизодов", animeEpisodes);
        InputTextStdString("Жанры (через \",\")", animeGenres);

        if (ImGui::Button("Загрузить модель")) {
            ImGUI_Analysis::LoadModel();
        }

        ImGui::SameLine();

        if (ImGui::Button("Предсказать из базы данных")) {
            if (_predictor->GetLoadedStatus()) {
                _predictor->PredictOnDatabase();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Предсказать новое")) {
            _predictor->PredictOnUI(animeName, animeType, animeEpisodes, animeGenres);
        }

        LogMenu();
        LoadStatus();

        ImGui::EndTabItem();
        return true;
    }
    return false;
}

void ImGUI_Analysis::LoadStatus() {
    const char* message = _predictor->GetLoadedStatus() ? "Модель загружена" : "Модель не загружена";
    ImVec4 color = _predictor->GetLoadedStatus() ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) : ImVec4(0.9f, 0.1f, 0.1f, 1.0f);

    ImVec2 windowPos = ImGui::GetIO().DisplaySize;
    ImVec2 padding = ImVec2(10, 10);
    windowPos.x -= padding.x;
    windowPos.y -= padding.y;

    ImGui::SetNextWindowBgAlpha(0.85f);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, ImVec2(1.0f, 1.0f));
    ImGui::Begin("ModelStatus", nullptr,
                 ImGuiWindowFlags_NoDecoration |
                 ImGuiWindowFlags_AlwaysAutoResize |
                 ImGuiWindowFlags_NoFocusOnAppearing |
                 ImGuiWindowFlags_NoNav |
                 ImGuiWindowFlags_NoInputs);

    ImGui::TextColored(color, "%s", message);
    ImGui::End();
}

void ImGUI_Analysis::LoadModel() {
    static std::string bufferName = "Analysis";

    try {
        _predictor->LoadMetadata("Training/model_export/metadata.json");
        _predictor->LoadModel("Training/model_export/model.tflite");

        _predictor->LoadDataBase("Training/out/anime.csv");
    } catch (const std::exception& ex) {
        LogManager::LogCustom(false, bufferName, std::string("Error: ") + ex.what() + " " + __LOGERROR__);
        LogManager::LogError(std::string("Error: ") + ex.what() + " " + __LOGERROR__);
    }
}

void ImGUI_Analysis::LogMenu() {
    LogManager& log = LogManager::GetInstance();

    static std::string bufferName = "Analysis";

    static size_t messageCount = 0;
    static std::string buffer;
    static size_t previousLogSize = 0;

    size_t counter = 0;
    for(const auto& it : log.GetCustomLogBuffer(bufferName)) {
        if (messageCount == log.GetCustomLogBuffer(bufferName).size()) { break; }
        if (counter < messageCount) { counter++; continue; }

        buffer += it + "\n";
        counter++;
        messageCount = counter;
    }

    ImGui::BeginChild("LogScrollRegion", ImVec2(-1, -1), true, ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::TextUnformatted(buffer.c_str());

    if (buffer.size() > previousLogSize) {
        ImGui::SetScrollHereY(1.0f);
        previousLogSize = buffer.size();
    }

    ImGui::EndChild();
}