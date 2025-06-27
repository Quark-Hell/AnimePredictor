#include "ImGUI_Main.h"

bool ImGUI_Main::Render() {
    // Устанавливаем флаги окна: без рамки, нельзя двигать, менять размер, закрывать
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoTitleBar |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("Anime Rating Predictor", nullptr, window_flags);

    if (ImGui::BeginTabBar("MainTabs")) {

        return true;
    }
    return false;
}