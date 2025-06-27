#ifndef ANIMEPREDICTOR_IMGUI_ANALYSIS_H
#define ANIMEPREDICTOR_IMGUI_ANALYSIS_H

#include "ImGUI/ImGUI_Entity.h"

class ImGUI_Analysis : public ImGUI_Entity {
public:
    explicit ImGUI_Analysis(const std::string& name) : ImGUI_Entity(name) {}

    bool Render() final;
};


#endif //ANIMEPREDICTOR_IMGUI_ANALYSIS_H
