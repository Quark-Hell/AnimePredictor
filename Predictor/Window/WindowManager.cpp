#include "WindowManager.h"

#include "ImGUI/ImGUI_Manager.h"
#include "Logger/LogManager.h"

#include <cmath>

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

WindowManager& WindowManager::GetInstance() {
    static WindowManager instance;
    return instance;
}

Window& WindowManager::GetWindow(float width, float height, std::string name) {
    static Window win (width, height, std::move(name));
    return win;
}

void WindowManager::SetWindowIcon(const std::string& path) {
    auto& win = WindowManager::GetWindow();
    auto& manager = WindowManager::GetInstance();

    manager._icon = std::make_unique<Texture>(path, false);
    manager._icon->Flip();
    manager._icon->Mirror();

    if (manager._icon->GetPixels()) {
        manager._images[0].width = manager._icon->GetWidth();
        manager._images[0].height = manager._icon->GetHeight();
        manager._images[0].pixels = manager._icon->GetPixels();

        glfwSetWindowIcon(win.window_ptr, 1, manager._images);
    } else {
        LogManager::LogError("Failed to load icon " + __LOGERROR__);
    }
}

void WindowManager::Start(float width, float height, std::string name) {
    // Setup GLFW
    glfwSetErrorCallback(glfw_error_callback);
    if(!glfwInit()){
        LogManager::LogCritical("Failed to initialize GLFW " + __LOGERROR__);
    }
    LogManager::LogInfo("GLFW Inited");

    // Setup OpenGL (GL 3.0+ Core Profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
#endif

    WindowManager::GetWindow(width, height, std::move(name));
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LogManager::LogCritical("Failed to initialize GLAD " + __LOGERROR__);
    }
    LogManager::LogInfo("GLAD Inited");

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

    LogManager::LogInfo("ImGUI Inited");
}

void WindowManager::ChangeFonts(const std::string& path) {
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();  // Очистить предыдущие шрифты

    io.Fonts->AddFontFromFileTTF(path.c_str(), 18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());

    //// Пересоздаём текстуру
    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_CreateFontsTexture();

    LogManager::LogInfo("Fonts changed to " + path);
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