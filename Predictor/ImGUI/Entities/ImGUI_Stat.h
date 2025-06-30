#ifndef ANIMEPREDICTOR_IMGUI_STAT_H
#define ANIMEPREDICTOR_IMGUI_STAT_H

#include "ImGUI/ImGUI_Entity.h"

class ImGUI_Stat : public ImGUI_Entity {
public:
    explicit ImGUI_Stat(const std::string& name) : ImGUI_Entity(name) {}

    bool Render() final;
};


#endif //ANIMEPREDICTOR_IMGUI_STAT_H
