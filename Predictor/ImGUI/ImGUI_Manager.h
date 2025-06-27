#ifndef ANIMEPREDICTOR_IMGUI_MANAGER_H
#define ANIMEPREDICTOR_IMGUI_MANAGER_H

#include "ImGUI_Entity.h"

#include <vector>
#include <memory>
#include <mutex>

struct Node {
    std::unique_ptr<ImGUI_Entity> entity;
    std::vector<std::unique_ptr<Node>> children;

    explicit Node(std::unique_ptr<ImGUI_Entity> ent) : entity(std::move(ent)) {}
};

class ImGUI_Manager {
public:
    static ImGUI_Manager& GetInstance();

    ImGUI_Manager(const ImGUI_Manager&) = delete;
    ImGUI_Manager& operator=(const ImGUI_Manager&) = delete;

    template<typename T, typename... Args>
    void AddEntity(const std::string& parent_name, Args&&... args) {
        static_assert(std::is_base_of<ImGUI_Entity, T>::value,
                      "T must inherit from ImGUI_Entity");

        auto new_entity = std::make_unique<T>(std::forward<Args>(args)...);
        std::lock_guard<std::mutex> lock(_mutex);

        if (parent_name.empty()) {
            _roots.emplace_back(std::make_unique<Node>(std::move(new_entity)));
        } else {
            Node* parent = FindNode(_roots, parent_name);
            if (parent) {
                parent->children.emplace_back(std::make_unique<Node>(std::move(new_entity)));
            } else {
                // Если родитель не найден — вставка на верхний уровень
                _roots.emplace_back(std::make_unique<Node>(std::move(new_entity)));
            }
        }
    }

    void RenderAll(int width, int height);
    Node* FindNode(const std::string& name);

private:
    ImGUI_Manager() = default;

    std::vector<std::unique_ptr<Node>> _roots;
    std::mutex _mutex;

    void RenderNode(Node& node);
    Node* FindNode(std::vector<std::unique_ptr<Node>>& nodes, const std::string& name);
};



#endif //ANIMEPREDICTOR_IMGUI_MANAGER_H
