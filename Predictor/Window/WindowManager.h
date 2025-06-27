#ifndef ANIMEPREDICTOR_WINDOWMANAGER_H
#define ANIMEPREDICTOR_WINDOWMANAGER_H

#include "Window.h"
#include <memory>

class WindowManager {
private:
    static inline std::string glsl_version = "#version 130";

private:
    static Window& GetWindow(float width = 1280, float height = 720, std::string name = "Unnamed");
    static int GetFlags();
    static void SetFlags();

    static void SetupImGui();

public:
    WindowManager() = delete;
    WindowManager(const WindowManager&) = delete;
    WindowManager(WindowManager&&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;
    WindowManager& operator=(WindowManager&&) = delete;

    static void Start(float width = 1280, float height = 720, std::string name = "Unnamed");
    static void Render();
    static bool ShouldClose();
    static void Shutdown();

    static void ChangeFonts(const std::string& path);

    [[nodiscard]] static RGBAColor GetClearColor();
    static void SetClearColor(RGBAColor newColor);
};


#endif //ANIMEPREDICTOR_WINDOWMANAGER_H
