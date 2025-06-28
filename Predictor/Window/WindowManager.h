#ifndef ANIMEPREDICTOR_WINDOWMANAGER_H
#define ANIMEPREDICTOR_WINDOWMANAGER_H

#include "Window.h"
#include "misc/Texture.h"

#include <memory>

class WindowManager {
private:
    std::unique_ptr<Texture> _icon;
    GLFWimage _images[1];

    static inline std::string glsl_version = "#version 130";

private:
    WindowManager() = default;

    static WindowManager& GetInstance();
    static Window& GetWindow(float width = 1280, float height = 720, std::string name = "Unnamed");
    static int GetFlags();
    static void SetFlags();

    static void SetupImGui();

public:
    WindowManager(const WindowManager&) = delete;
    WindowManager(WindowManager&&) = delete;
    WindowManager& operator=(const WindowManager&) = delete;
    WindowManager& operator=(WindowManager&&) = delete;

    static void Start(float width = 1280, float height = 720, std::string name = "Predictor");
    static void Render();
    static bool ShouldClose();
    static void Shutdown();

    static void ChangeFonts(const std::string& path);

    [[nodiscard]] static RGBAColor GetClearColor();
    static void SetClearColor(RGBAColor newColor);

    static void SetWindowIcon(const std::string& path);
};


#endif //ANIMEPREDICTOR_WINDOWMANAGER_H
