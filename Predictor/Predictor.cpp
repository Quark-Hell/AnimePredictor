#include "Predictor.h"
#include "Window/WindowManager.h"
#include "Model/Loader.h"

#include "ImGUI/ImGUI_Manager.h"

#include "ImGUI/Entities/ImGUI_Main.h"
#include "ImGUI/Entities/ImGUI_Analysis.h"
#include "ImGUI/Entities/ImGUI_Training.h"
#include "ImGUI/Entities/ImGUI_EndMain.h"
#include "ImGUI/Entities/ImGUI_Logger.h"

int main() {
    //Loader loader;
    //loader.Test();

    auto& mgr = ImGUI_Manager::GetInstance();

    mgr.AddEntity<ImGUI_Main>("", "Main");
    mgr.AddEntity<ImGUI_Training>("Main", "Training");
    mgr.AddEntity<ImGUI_Analysis>("Main", "Analysis");
    mgr.AddEntity<ImGUI_Logger>("Main", "Logger");
    mgr.AddEntity<ImGUI_EndMain>("Main", "EndMain");

    WindowManager::Start();
    WindowManager::ChangeFonts("assets/fonts/Roboto-VariableFont.ttf");
    WindowManager::SetClearColor(RGBAColor{ 0.5f, 0.5f, 0.5f, 1.0f} );

    while (!WindowManager::ShouldClose()) {
        WindowManager::Render();
    }

    WindowManager::Shutdown();

    return 0;
}
