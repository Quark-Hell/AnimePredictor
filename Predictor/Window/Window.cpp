#include "Window.h"

#include <cstdio>

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

Window::Window(float width, float height, std::string name) {
    _widht = width;
    _height = height;

    _name = std::move(name);

    SetupGLFW();
}

Window::~Window() {
    glfwDestroyWindow(window_ptr);
    glfwTerminate();
}

void Window::SetupGLFW() {
    // Setup GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        isInit = false;

    // Setup OpenGL (GL 3.0+ Core Profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
#endif
    isInit = true;
}

GLFWwindow* Window::CreateWindow() {
    if(isInit == false){
        return nullptr;
    }

    // Create window
    window_ptr = glfwCreateWindow(_widht, _height, _name.c_str(), NULL, NULL);
    glfwMakeContextCurrent(window_ptr);
    glfwSwapInterval(1); // Enable vsync
    return window_ptr;
}