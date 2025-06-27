#include "ImGUI_Analysis.h"

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
        InputTextStdString("Жанры (через |)", animeGenres);

        if (ImGui::Button("Предсказать из базы данных")) {
            // TODO: Добавь логику поиска по базе и предсказания
        }

        ImGui::SameLine();

        if (ImGui::Button("Предсказать новое")) {
            // TODO: Добавь логику создания новой записи и предсказания
        }

        ImGui::EndTabItem();
        return true;
    }
    return false;
}