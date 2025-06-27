project(Predictor)
cmake_minimum_required(VERSION 3.22)

if(POLICY CMP0169)
    cmake_policy(SET CMP0169 NEW)
endif()

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Predictor)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/ImGUI)
set(IMGUI_BACKENDS ${IMGUI_DIR}/backends)
set(STB_DIR ${CMAKE_SOURCE_DIR}/external/stb)
set(GLAD_DIR ${CMAKE_SOURCE_DIR}/external/glad)

set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_BACKENDS}/imgui_impl_glfw.cpp
        ${IMGUI_BACKENDS}/imgui_impl_opengl3.cpp
)

set(PREDICTOR_SOURCE ${CMAKE_SOURCE_DIR}/)

set(GLAD_SOURCE
        ${GLAD_DIR}/src/glad.c
)
set(GLAD_HEADERS
        ${GLAD_DIR}/include
)

include(FetchContent)

# Загрузка TensorFlow Lite C++ с упрощенной сборкой
FetchContent_Declare(
        tensorflow-lite
        GIT_REPOSITORY https://github.com/tensorflow/tensorflow.git
        GIT_TAG v2.15.0
        GIT_SHALLOW TRUE
        SOURCE_SUBDIR tensorflow/lite
)

# Настройки для минимальной сборки TensorFlow Lite
set(TFLITE_ENABLE_INSTALL OFF CACHE BOOL "Enable install" FORCE)
set(TFLITE_ENABLE_TESTS OFF CACHE BOOL "Enable tests" FORCE)
set(TFLITE_ENABLE_EXAMPLES OFF CACHE BOOL "Enable examples" FORCE)
set(TFLITE_ENABLE_BENCHMARK_MODEL OFF CACHE BOOL "Enable benchmark model" FORCE)
set(TFLITE_ENABLE_XNNPACK OFF CACHE BOOL "Enable XNNPACK" FORCE)

FetchContent_MakeAvailable(tensorflow-lite)

# Поиск или загрузка nlohmann/json
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann/json not found in system. Downloading...")
    FetchContent_Declare(
            nlohmann_json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG v3.11.3
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(nlohmann_json)
endif()

# GLFW
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.3.8
)
FetchContent_MakeAvailable(glfw)

# FlatBuffers
FetchContent_Declare(
        flatbuffers
        GIT_REPOSITORY https://github.com/google/flatbuffers.git
        GIT_TAG v23.5.26
)
FetchContent_MakeAvailable(flatbuffers)

# ==== Find OpenGL ====
find_package(OpenGL REQUIRED)

# ==== Add Executable ====
add_executable(Predictor
        Predictor.cpp

        Logger/LogManager.cpp

        Parser/Parser.cpp
        Parser/Exporter.cpp

        Parser/Tables/AnimeTable.cpp
        Parser/Tables/UserRatingTable.cpp

        Window/Window.cpp
        Window/WindowManager.cpp

        misc/RGBAColor.cpp
        misc/Texture.cpp

        Model/Model.cpp
        Model/Loader.cpp

        ImGUI/ImGUI_Manager.cpp
        ImGUI/Entities/ImGUI_Training.cpp
        ImGUI/Entities/ImGUI_StartTraining.cpp
        ImGUI/Entities/ImGUI_Analysis.cpp
        ImGUI/Entities/ImGUI_Main.cpp
        ImGUI/Entities/ImGUI_EndMain.cpp
        ImGUI/Entities/ImGUI_Logger.cpp

        ${IMGUI_SOURCES}
        ${GLAD_SOURCE}
)

# ==== Include Directories ====
target_include_directories(Predictor PRIVATE
        ${PREDICTOR_SOURCE}
        ${IMGUI_DIR}
        ${IMGUI_BACKENDS}
        ${STB_DIR}
        ${GLAD_HEADERS}
        ${tensorflow-lite_SOURCE_DIR}
        ${tensorflow-lite_SOURCE_DIR}/..
)

# ==== Link Libraries ====
target_link_libraries(Predictor PRIVATE
        tensorflow-lite
        nlohmann_json::nlohmann_json
        pthread
        dl
        glfw
        OpenGL::GL
        flatbuffers
)

# ==== Windows-specific settings ====
if (WIN32)
    target_link_libraries(Predictor PRIVATE opengl32)
endif()

# Дополнительные определения компилятора
target_compile_definitions(Predictor PRIVATE
        TFLITE_ENABLE_RUY=1
)

function(copy_folder_after_build SRC_DIR DST_DIR TARGET_NAME)
    add_custom_command(
            TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${SRC_DIR}"
            "${DST_DIR}"
            COMMENT "Assets copied"
    )
endfunction()

copy_folder_after_build(
        "${CMAKE_SOURCE_DIR}/assets"
        "${CMAKE_CURRENT_BINARY_DIR}/assets"
        Predictor
)

copy_folder_after_build(
        "${CMAKE_SOURCE_DIR}/Training"
        "${CMAKE_CURRENT_BINARY_DIR}/Training"
        Predictor
)