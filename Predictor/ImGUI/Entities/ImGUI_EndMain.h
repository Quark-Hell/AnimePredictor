#ifndef ANIMEPREDICTOR_IMGUI_ENDMAIN_H
#define ANIMEPREDICTOR_IMGUI_ENDMAIN_H

#include "ImGUI/ImGUI_Entity.h"

class ImGUI_EndMain : public ImGUI_Entity {
public:
    explicit ImGUI_EndMain(const std::string& name) : ImGUI_Entity(name) {}

    bool Render() final;
};


#endif //ANIMEPREDICTOR_IMGUI_ENDMAIN_H
