#include "ImGUI_Logger.h"

#include "Logger/LogManager.h"

bool ImGUI_Logger::Render() {
    if (ImGui::BeginTabItem("Логирование")) {
        // Буфер, необходимый для отображения
        static std::string buffer;
        static size_t previousLogSize = 0;

        ParseMessages(buffer);

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

void ImGUI_Logger::ParseMessages(std::string& buffer) {
    LogManager& log = LogManager::GetInstance();

    static size_t infoMessageCount = 0;
    size_t counter = 0;
    for(const auto& it : log.GetInfoLogBuffer()) {
        if (infoMessageCount == log.GetInfoLogBuffer().size()) { break; }
        if (counter < infoMessageCount) { counter++; continue; }

        buffer += it + "\n";
        counter++;
        infoMessageCount = counter;
    }

    static size_t warningMessageCount = 0;
    counter = 0;
    for(const auto& it : log.GetInfoLogBuffer()) {
        if (warningMessageCount == log.GetInfoLogBuffer().size()) { break; }
        if (counter < warningMessageCount) { counter++; continue; }

        buffer += it + "\n";
        counter++;
        warningMessageCount = counter;
    }

    static size_t errorMessageCount = 0;
    counter = 0;
    for(const auto& it : log.GetInfoLogBuffer()) {
        if (errorMessageCount == log.GetInfoLogBuffer().size()) { break; }
        if (counter < errorMessageCount) { counter++; continue; }

        buffer += it + "\n";
        counter++;
        errorMessageCount = counter;
    }

    static size_t criticalMessageCount = 0;
    counter = 0;
    for(const auto& it : log.GetInfoLogBuffer()) {
        if (criticalMessageCount == log.GetInfoLogBuffer().size()) { break; }
        if (counter < criticalMessageCount) { counter++; continue; }

        buffer += it + "\n";
        counter++;
        criticalMessageCount = counter;
    }
}