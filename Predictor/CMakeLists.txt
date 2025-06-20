project(Predictor)
cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Predictor)

# ==== Dear ImGui source files ====
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/ImGUI)
set(IMGUI_BACKENDS ${IMGUI_DIR}/backends)

set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_BACKENDS}/imgui_impl_glfw.cpp
        ${IMGUI_BACKENDS}/imgui_impl_opengl3.cpp
)

# ==== GLFW with FetchContent if needed ====
include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.3.8
)

FetchContent_MakeAvailable(glfw)

# ==== Find OpenGL ====
find_package(OpenGL REQUIRED)

# ==== Add Executable ====
add_executable(Predictor
        Predictor.cpp
        Window/Window.cpp

        ${IMGUI_SOURCES}
)

# ==== Include Directories ====
target_include_directories(Predictor PRIVATE
        ${IMGUI_DIR}
        ${IMGUI_BACKENDS}
)

# ==== Link Libraries ====
target_link_libraries(Predictor PRIVATE glfw OpenGL::GL)

# ==== Windows-specific settings ====
if (WIN32)
    target_link_libraries(Predictor PRIVATE opengl32)
endif()

## Функция для определения параметров TensorFlow
#function(get_tensorflow_params)
#    if(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 8)
#        set(TF_PLATFORM "cpu-windows-x86_64" PARENT_SCOPE)
#        set(TF_EXTENSION "zip" PARENT_SCOPE)
#        set(TF_LIB_NAME "tensorflow" PARENT_SCOPE)
#    elseif(APPLE)
#        if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64")
#            set(TF_PLATFORM "cpu-darwin-arm64" PARENT_SCOPE)
#        else()
#            set(TF_PLATFORM "cpu-darwin-x86_64" PARENT_SCOPE)
#        endif()
#        set(TF_EXTENSION "tar.gz" PARENT_SCOPE)
#        set(TF_LIB_NAME "tensorflow" PARENT_SCOPE)
#    elseif(UNIX)
#        if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
#            set(TF_PLATFORM "cpu-linux-aarch64" PARENT_SCOPE)
#        else()
#            set(TF_PLATFORM "cpu-linux-x86_64" PARENT_SCOPE)
#        endif()
#        set(TF_EXTENSION "tar.gz" PARENT_SCOPE)
#        set(TF_LIB_NAME "tensorflow" PARENT_SCOPE)
#    else()
#        message(FATAL_ERROR "Unsupported platform")
#    endif()
#endfunction()

# Получаем параметры для текущей платформы
#get_tensorflow_params()

## Скачиваем TensorFlow
#FetchContent_Declare(
#        tensorflow
#        URL "https://storage.googleapis.com/tensorflow/libtensorflow/libtensorflow-${TF_PLATFORM}-2.13.0.${TF_EXTENSION}"
#)
#
#FetchContent_MakeAvailable(tensorflow)

## Подключаем заголовки и библиотеки
#target_include_directories(Parser PRIVATE "${tensorflow_SOURCE_DIR}/include")

## Находим библиотеку TensorFlow
#find_library(TENSORFLOW_LIB
#        NAMES ${TF_LIB_NAME} tensorflow
#        PATHS "${tensorflow_SOURCE_DIR}/lib"
#        NO_DEFAULT_PATH
#)
#
#if(NOT TENSORFLOW_LIB)
#    message(FATAL_ERROR "TensorFlow library not found in ${tensorflow_SOURCE_DIR}/lib")
#endif()

#target_link_libraries(Parser ${TENSORFLOW_LIB})

## Копируем динамические библиотеки
#if(WIN32)
#    file(GLOB TF_DLLS "${tensorflow_SOURCE_DIR}/lib/*.dll")
#    foreach(dll ${TF_DLLS})
#        add_custom_command(TARGET Parser POST_BUILD
#                COMMAND ${CMAKE_COMMAND} -E copy_if_different
#                ${dll} $<TARGET_FILE_DIR:Parser>
#        )
#    endforeach()
#elseif(UNIX AND NOT APPLE)
#    file(GLOB TF_SOS "${tensorflow_SOURCE_DIR}/lib/*.so*")
#    foreach(so ${TF_SOS})
#        add_custom_command(TARGET Parser POST_BUILD
#                COMMAND ${CMAKE_COMMAND} -E copy_if_different
#                ${so} $<TARGET_FILE_DIR:Parser>
#        )
#    endforeach()
#elseif(APPLE)
#    file(GLOB TF_DYLIBS "${tensorflow_SOURCE_DIR}/lib/*.dylib")
#    foreach(dylib ${TF_DYLIBS})
#        add_custom_command(TARGET Parser POST_BUILD
#                COMMAND ${CMAKE_COMMAND} -E copy_if_different
#                ${dylib} $<TARGET_FILE_DIR:Parser>
#        )
#    endforeach()
#endif()
