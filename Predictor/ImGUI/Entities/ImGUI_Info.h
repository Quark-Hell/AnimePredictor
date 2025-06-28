#ifndef ANIMEPREDICTOR_IMGUI_INFO_H
#define ANIMEPREDICTOR_IMGUI_INFO_H

#include "ImGUI/ImGUI_Entity.h"

#include <vector>

class ImGUI_Info : public ImGUI_Entity {
public:
    explicit ImGUI_Info(const std::string& name);

    bool Render() override;

private:
    std::vector<std::string> genreClasses;
    std::vector<std::string> typeClasses;
    bool isLoaded = false;

    void LoadMetadata();
};


#endif //ANIMEPREDICTOR_IMGUI_INFO_H
