#ifndef ANIMEPREDICTOR_IMGUI_ANALYSIS_H
#define ANIMEPREDICTOR_IMGUI_ANALYSIS_H

#include "ImGUI/ImGUI_Entity.h"
#include "Model/Model.h"

#include <memory>

class ImGUI_Analysis : public ImGUI_Entity {
private:
    std::unique_ptr<Model> _predictor;

public:
    explicit ImGUI_Analysis(const std::string& name) : ImGUI_Entity(name) {
        _predictor = std::make_unique<Model>();
    }

    bool Render() final;

private:
    void LoadModel();

    void LogMenu();
    void LoadStatus();
};


#endif //ANIMEPREDICTOR_IMGUI_ANALYSIS_H
