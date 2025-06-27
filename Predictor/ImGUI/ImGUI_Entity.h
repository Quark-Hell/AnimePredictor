#ifndef ANIMEPREDICTOR_IMGUI_ENTITY_H
#define ANIMEPREDICTOR_IMGUI_ENTITY_H

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <string>

inline bool InputTextStdString(const char* label, std::string& str, ImGuiInputTextFlags flags = 0)
{
    // Добавим флаг CallbackResize, чтобы поддерживалась динамическая длина
    flags |= ImGuiInputTextFlags_CallbackResize;

    return ImGui::InputText(label, str.data(), str.capacity() + 1, flags,
                            [](ImGuiInputTextCallbackData* data) {
                                if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
                                    auto* str = reinterpret_cast<std::string*>(data->UserData);
                                    str->resize(data->BufTextLen);
                                    data->Buf = str->data();
                                }
                                return 0;
                            },
                            static_cast<void*>(&str)
    );
}

class ImGUI_Entity {
public:
    explicit ImGUI_Entity(const std::string& entityName) {
        name = entityName;
    }

    virtual bool Render() = 0;
    virtual ~ImGUI_Entity() = default;

    std::string name;
};

#endif //ANIMEPREDICTOR_IMGUI_ENTITY_H
