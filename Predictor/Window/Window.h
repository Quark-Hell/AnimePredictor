#ifndef ANIMEPREDICTOR_WINDOW_H
#define ANIMEPREDICTOR_WINDOW_H

#include "GLFW/glfw3.h"
#include <string>

class Window {
private:
    float _widht = 0;
    float _height = 0;

    std::string _name = "Unnamed";

    bool isInit = false;

    void SetupGLFW();

public:
    GLFWwindow* window_ptr;

    explicit Window(float width = 1280, float height = 720, std::string name = "Unnamed");
    ~Window();

    GLFWwindow* CreateWindow();

};


#endif //ANIMEPREDICTOR_WINDOW_H
