#include "ImGUI_Manager.h"

ImGUI_Manager& ImGUI_Manager::GetInstance() {
    static ImGUI_Manager instance;
    return instance;
}

void ImGUI_Manager::RenderAll(int width, int height) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Настраиваем главное окно
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(width), static_cast<float>(height)));

    std::lock_guard<std::mutex> lock(_mutex);
    for (auto& node : _roots) {
        RenderNode(*node);
    }

    ImGui::End();
    ImGui::Render();
}

void ImGUI_Manager::RenderNode(Node& node) {
    if (!node.entity) return;
    bool go_deeper = node.entity->Render();
    if (go_deeper) {
        for (auto& child : node.children) {
            RenderNode(*child);
        }
    }
}

Node* ImGUI_Manager::FindNode(std::vector<std::unique_ptr<Node>>& nodes, const std::string& name) {
    for (auto& node : nodes) {
        if (node->entity && node->entity->name == name) return node.get();
        if (!node->children.empty()) {
            Node* found = FindNode(node->children, name);
            if (found) return found;
        }
    }
    return nullptr;
}