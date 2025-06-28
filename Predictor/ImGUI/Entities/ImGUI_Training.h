#ifndef ANIMEPREDICTOR_IMGUI_TRAINING_H
#define ANIMEPREDICTOR_IMGUI_TRAINING_H

#include "ImGUI/ImGUI_Entity.h"

#include <atomic>
#include <thread>

class ImGUI_Training : public ImGUI_Entity {
private:
    std::atomic<bool> trainingInProgress = false;
    std::atomic<bool> trainingCompleted = false;
    std::thread trainingThread;

    std::string animePath = "assets/data/anime.csv";
    std::string ratingPath = "assets/data/rating.csv";

public:
    explicit ImGUI_Training(const std::string& name) : ImGUI_Entity(name) {}

    bool Render() final;

private:
    void TrainButton();

    void TrainModel(const std::string& animePath, const std::string& ratingPath);
    void RunDocker();

    void LogMenu();
};


#endif //ANIMEPREDICTOR_IMGUI_TRAINING_H
