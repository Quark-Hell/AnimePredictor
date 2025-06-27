#ifndef ANIMEPREDICTOR_IMGUI_TRAINING_H
#define ANIMEPREDICTOR_IMGUI_TRAINING_H

#include "ImGUI/ImGUI_Entity.h"

class ImGUI_Training : public ImGUI_Entity {
public:
    explicit ImGUI_Training(const std::string& name) : ImGUI_Entity(name) {}

    bool Render() final;

private:
    void TrainModel(const std::string& animePath, const std::string& ratingPath);
};


#endif //ANIMEPREDICTOR_IMGUI_TRAINING_H
