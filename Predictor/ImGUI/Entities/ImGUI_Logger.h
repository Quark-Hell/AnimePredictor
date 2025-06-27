#ifndef ANIMEPREDICTOR_IMGUI_LOGGER_H
#define ANIMEPREDICTOR_IMGUI_LOGGER_H

#include "ImGUI/ImGUI_Entity.h"
#include "Logger/LogManager.h"

class ImGUI_Logger : public ImGUI_Entity {
public:
    explicit ImGUI_Logger(const std::string& name) : ImGUI_Entity(name) {}

    bool Render() final;
};


#endif //ANIMEPREDICTOR_IMGUI_LOGGER_H
