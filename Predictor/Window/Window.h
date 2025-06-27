#ifndef ANIMEPREDICTOR_WINDOW_H
#define ANIMEPREDICTOR_WINDOW_H

#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include "misc/RGBAColor.h"

#include <string>

class Window {
    friend class WindowManager;

private:
    float _width = 0;
    float _height = 0;

    RGBAColor _clearColor {0.05f, 0.05f, 0.05f, 1.0f};

    std::string _name = "Unnamed";

    GLFWwindow* window_ptr;

private:
    explicit Window(float width = 1280, float height = 720, std::string name = "Unnamed");
    void CreateWindow();
};


#endif //ANIMEPREDICTOR_WINDOW_H
