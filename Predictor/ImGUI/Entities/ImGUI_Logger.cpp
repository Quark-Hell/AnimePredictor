#include "ImGUI_Logger.h"

bool ImGUI_Logger::Render() {
    if (ImGui::BeginTabItem("Логирование")) {
        //const std::string& logs = LogManager::GetInstance().GetLogs();

        // Буфер, необходимый для отображения
        static std::string buffer;
        static int previousLogSize = 0;
        buffer = "logs";

        // Начинаем Child-зону для скроллинга
        ImGui::BeginChild("LogScrollRegion", ImVec2(-1, -1), true, ImGuiWindowFlags_HorizontalScrollbar);

        ImGui::TextUnformatted(buffer.c_str());

        // Автоскролл, если добавлены новые данные
        if ((int)buffer.size() > previousLogSize) {
            ImGui::SetScrollHereY(1.0f);  // Прокрутка к низу
            previousLogSize = buffer.size();
        }

        ImGui::EndChild();
        ImGui::EndTabItem();
        return true;
    }
    return false;
}