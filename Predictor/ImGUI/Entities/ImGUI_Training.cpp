#include "ImGUI_Training.h"

#include "ImGUI/Entities/ImGUI_StartTraining.h"
#include "ImGUI/ImGUI_Manager.h"

bool ImGUI_Training::Render() {
    if (ImGui::BeginTabItem("Обучение")) {
        ImGui::Text("Интерфейс для обучения модели");
        ImGui::Text("Внимание: для обучения модели на своих данных у Вас должен быть установлен Docker");
        ImGui::Separator();

        animePath.reserve(256);
        ratingPath.reserve(256);

        ImGUI_Manager& instance = ImGUI_Manager::GetInstance();
        auto node = instance.FindNode("StartTraining");
        if (node != nullptr) {
            ImGUI_Entity* train = node->entity.get();
            ImGUI_StartTraining* trainButton = reinterpret_cast<ImGUI_StartTraining*>(train);

            trainButton->animePath = animePath;
            trainButton->ratingPath = ratingPath;
        }

        InputTextStdString("Путь к anime.csv", animePath);
        InputTextStdString("Путь к rating.csv", ratingPath);

        ImGui::EndTabItem();
        return true;
    }
    return false;
}