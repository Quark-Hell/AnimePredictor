#include "WindowManager.h"

#include "misc/Texture.h"

#include "ImGUI/ImGUI_Manager.h"

#include <iostream>
#include <cmath>

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

Window& WindowManager::GetWindow(float width, float height, std::string name) {
    static Window* win (new Window(width, height, std::move(name)));
    return *win;
}

void WindowManager::Start(float width, float height, std::string name) {
    // Setup GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if(!glfwInit()){
        std::cerr << "Not init";
    }

    // Setup OpenGL (GL 3.0+ Core Profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
#endif

    WindowManager::GetWindow(width, height, std::move(name));
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
    }

    SetupImGui();
}

void WindowManager::SetupImGui() {
    auto& win = WindowManager::GetWindow();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(win.window_ptr, true);
    ImGui_ImplOpenGL3_Init(glsl_version.c_str());
}

void WindowManager::ChangeFonts(const std::string& path) {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();  // Очистить предыдущие шрифты

    io.Fonts->AddFontFromFileTTF(path.c_str(), 18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());

    //// Пересоздаём текстуру
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_CreateFontsTexture();
}

void WindowManager::Render() {
    auto& win = WindowManager::GetWindow();

    glfwPollEvents();

    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(win.window_ptr, &width, &height);

    win._width = width;
    win._height = height;

    auto& mgr = ImGUI_Manager::GetInstance();
    mgr.RenderAll(width, height);

    //Texture myTexture("assets/test.jpg");
    //myTexture.Bind();
    //ImGui::Image(
    //        (ImTextureID)(intptr_t)myTexture.GetTextureID(),
    //        ImVec2(myTexture.GetWidth(), myTexture.GetHeight()),
    //        ImVec2(0, 1), // uv0
    //        ImVec2(1, 0)  // uv1 — инвертируем, чтобы перевернуть текстуру
    //);
    //myTexture.Unbind();

    // Rendering

    glViewport(0, 0, win._width, win._height);
    glClearColor(win._clearColor.GetR(), win._clearColor.GetG(), win._clearColor.GetB(), win._clearColor.GetA());
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(win.window_ptr);
}

bool WindowManager::ShouldClose() {
    auto& win = WindowManager::GetWindow();
    return glfwWindowShouldClose(win.window_ptr);
}

void WindowManager::Shutdown() {
    auto& win = WindowManager::GetWindow();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(win.window_ptr);
    glfwTerminate();
}

RGBAColor WindowManager::GetClearColor() {
    auto& win = WindowManager::GetWindow();
    return win._clearColor;
}

void WindowManager::SetClearColor(RGBAColor newColor) {
    auto& win = WindowManager::GetWindow();
    win._clearColor = newColor;
}