#include "ImGUI_Stat.h"

#include <filesystem>
#include <fstream>

bool ImGUI_Stat::Render() {
    if (ImGui::BeginTabItem("Статистика модели")) {
        // Буфер, необходимый для отображения
        static std::string buffer;
        static size_t previousLogSize = 0;
        static bool loaded = false;

        // Загружаем файл только один раз или если он был обновлён
        if (!loaded) {
            static const std::string filePath = "Training/model_export/results.txt";
            if (std::filesystem::exists(filePath)) {
                std::ifstream in(filePath);
                std::stringstream ss;
                ss << in.rdbuf();
                buffer = ss.str();
                previousLogSize = buffer.size();
                loaded = true;
            } else {
                buffer = "Файл model_export/results.txt не найден.";
            }
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
        ImGui::EndTabItem();

        return true;
    }

    return false;
}