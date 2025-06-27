#ifndef ANIMEPREDICTOR_IMGUI_TRAINING_H
#define ANIMEPREDICTOR_IMGUI_TRAINING_H

#include "ImGUI/ImGUI_Entity.h"

class ImGUI_Training : public ImGUI_Entity {
private:
    std::string animePath = "assets/data/anime.csv";
    std::string ratingPath = "assets/data/rating.csv";

public:
    explicit ImGUI_Training(const std::string& name) : ImGUI_Entity(name) {}

    bool Render() final;
};


#endif //ANIMEPREDICTOR_IMGUI_TRAINING_H
