#pragma once

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <chrono>

#define GLFW_INCLUDE_NONE

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/string_cast.hpp>

#include <imgui/imgui.h>
#include <imgui_internal.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <Utility/math.h>
#include <Utility/config.h>
#include <Utility/dbgprint.h>

#include <Renderer/OBJModel.h>
#include <Renderer/Shader.h>
#include <Renderer/Camera.h>
#include <Renderer/Cube.h>

#include <Structs/SlyLevelFile.h>

#include <Gui/DebugInformation.h>
#include <Gui/RendererWindow.h>
#include <Gui/ModelBrowser.h>
#include <Gui/ModelViewer.h>

#include <Editor.h>

static void key_callback(GLFWwindow*, int, int, int, int);
static void error_callback(int, const char* desc);
static void size_callback(GLFWwindow*, int, int);
static void cursor_position_callback(GLFWwindow*, double, double);
static void handle_input();
static void scroll_callback(GLFWwindow*, double, double);
static void mouse_button_callback(GLFWwindow*, int, int, int);

static int current_cursor_mode{ GLFW_CURSOR_DISABLED };