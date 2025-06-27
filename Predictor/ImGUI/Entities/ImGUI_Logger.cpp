#include "ImGUI_Logger.h"

#include "Logger/LogManager.h"

bool ImGUI_Logger::Render() {
    if (ImGui::BeginTabItem("Логирование")) {
        LogManager& log = LogManager::GetInstance();

        // Буфер, необходимый для отображения
        static size_t messageCount = 0;
        static std::string buffer;
        static size_t previousLogSize = 0;

        size_t counter = 0;
        for(const auto& it : log.GetLogBuffer()) {
            if (messageCount == log.GetLogBuffer().size()) { break; }
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
        ImGui::EndTabItem();
        return true;
    }
    return false;
}