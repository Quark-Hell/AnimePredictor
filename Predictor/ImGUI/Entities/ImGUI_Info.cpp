#include "ImGUI_Info.h"

#include "Logger/LogManager.h"

#include <fstream>
#include <nlohmann/json.hpp>

ImGUI_Info::ImGUI_Info(const std::string& name) : ImGUI_Entity(name) {
    LoadMetadata();
}

void ImGUI_Info::LoadMetadata() {
    const static std::string path = "Training/model_export/metadata.json";
    std::ifstream file(path);
    if (!file.is_open()) {
        LogManager::LogError("Failed to open metadata file: " + path + " " + __LOGERROR__);
        return;
    }

    try {
        nlohmann::json metadata;
        file >> metadata;

        for (const auto& genre : metadata["genre_classes"]) {
            genreClasses.emplace_back(genre);
        }

        for (const auto& type : metadata["type_classes"]) {
            typeClasses.emplace_back(type);
        }

        isLoaded = true;
    } catch (const std::exception& ex) {
        LogManager::LogError(std::string("Error parsing metadata.json: ") + ex.what() + " " + __LOGERROR__);
    }
}

bool ImGUI_Info::Render() {
    if (ImGui::BeginTabItem("Справка")) {
        ImGui::TextWrapped("Справочная информация по допустимым жанрам и типам аниме:");
        ImGui::Separator();

        if (!isLoaded) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Не удалось загрузить metadata.json");
        } else {
            ImGui::Text("Типы аниме:");
            for (const auto& type : typeClasses) {
                ImGui::BulletText("%s", type.c_str());
            }

            ImGui::Spacing();
            ImGui::Text("Доступные жанры:");
            for (const auto& genre : genreClasses) {
                ImGui::BulletText("%s", genre.c_str());
            }
        }

        ImGui::EndTabItem();
        return true;
    }
    return false;
}