#pragma once

#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <chrono>

#define GLEW_STATIC
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

#include <Utility/math.h>

#include <Renderer/OBJModel.h>
#include <Renderer/Shader.h>
#include <Renderer/Camera.h>
#include <Renderer/Cube.h>
#include <Structs/SlyLevelFile.h>


//#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "glfw3.lib")
//#pragma comment(lib, "glew32s.lib")

//int main(int argc, const char* argv[]);

std::vector<RenderedWorldObject*> g_objects;

static bool g_renderer_shown{ true };
static bool g_debug_information_shown{ true };

static bool g_draw_wireframe_only{ false };

static void render_gui();
static void key_callback(GLFWwindow*, int, int, int, int);
static void error_callback(int, const char* desc);
static void size_callback(GLFWwindow*, int, int);
static void cursor_position_callback(GLFWwindow*, double, double);
static void handle_input();
static void scroll_callback(GLFWwindow*, double, double);
static void mouse_button_callback(GLFWwindow*, int, int, int);
static void render_renderer();

static void make_render_texture(int width, int height);

static int current_cursor_mode{ GLFW_CURSOR_DISABLED };
static int currently_selected_mesh = -1;

template <typename... T>
inline static void dbgprint(const char* a, T ...args)  {
#ifndef NDEBUG
	printf(a, args...);
#endif
}

inline static void dbgprintf(const char* a) {
#ifndef NDEBUG
	printf(a);
#endif
}