#include "Predictor.h"
#include "Window/WindowManager.h"

#include "ImGUI/ImGUI_Manager.h"

#include "ImGUI/Entities/ImGUI_Main.h"
#include "ImGUI/Entities/ImGUI_Analysis.h"
#include "ImGUI/Entities/ImGUI_Training.h"
#include "ImGUI/Entities/ImGUI_StartTraining.h"
#include "ImGUI/Entities/ImGUI_EndMain.h"
#include "ImGUI/Entities/ImGUI_Logger.h"

#include "Logger/LogManager.h"

int main() {
    try {
        LogManager::LogInfo("Start program");

        auto& mgr = ImGUI_Manager::GetInstance();

        mgr.AddEntity<ImGUI_Main>("", "Main");

        mgr.AddEntity<ImGUI_Training>("Main", "Training");
        mgr.AddEntity<ImGUI_StartTraining>("Training", "StartTraining");

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
    }
    catch (const std::exception& ex) {
        LogManager::LogCritical(std::string("Program was crashed with: ") + ex.what() + " " + __LOGERROR__);
    }

    return 0;
}
