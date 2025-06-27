#include "Window.h"

#include <cstdio>

Window::Window(float width, float height, std::string name) {
    _width = width;
    _height = height;

    _name = std::move(name);

    Window::CreateWindow();
}

void Window::CreateWindow() {
    window_ptr = glfwCreateWindow(_width, _height, _name.c_str(), NULL, NULL);
    glfwMakeContextCurrent(window_ptr);
    glfwSwapInterval(1); // Enable vsync
}