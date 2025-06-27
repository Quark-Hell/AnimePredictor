#ifndef ANIMEPREDICTOR_IMGUI_STARTTRAINING_H
#define ANIMEPREDICTOR_IMGUI_STARTTRAINING_H

#include "ImGUI/ImGUI_Entity.h"

#include <atomic>
#include <thread>

class ImGUI_StartTraining : public ImGUI_Entity {
private:
    std::atomic<bool> trainingInProgress = false;
    std::atomic<bool> trainingCompleted = false;
    std::thread trainingThread;

public:
    std::string animePath = "assets/data/anime.csv";
    std::string ratingPath = "assets/data/rating.csv";

public:
    explicit ImGUI_StartTraining(const std::string& name) : ImGUI_Entity(name) {}

    bool Render() final;

private:
    void TrainModel(const std::string& animePath, const std::string& ratingPath);
    void RunDocker();
};


#endif //ANIMEPREDICTOR_IMGUI_STARTTRAINING_H
