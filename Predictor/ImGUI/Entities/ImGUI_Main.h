#ifndef ANIMEPREDICTOR_IMGUI_MAIN_H
#define ANIMEPREDICTOR_IMGUI_MAIN_H

#include "ImGUI/ImGUI_Entity.h"

class ImGUI_Main : public ImGUI_Entity {
public:
    explicit ImGUI_Main(const std::string& name) : ImGUI_Entity(name) {}

    bool Render() final;
};


#endif //ANIMEPREDICTOR_IMGUI_MAIN_H
