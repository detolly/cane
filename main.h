#pragma once

#include <windows.h>

#define GLEW_STATIC
#define GLFW_INCLUDE_NONE

#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/string_cast.hpp>
#include <OBJModel.h>

#include <iostream>

#include <Shader.h>
#include <Camera.h>
#include <Cube.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "glew32s.lib")

//int main(int argc, const char* argv[]);
static void key_callback(GLFWwindow*, int, int, int, int);
static void error_callback(int, const char* desc);
static void size_callback(GLFWwindow*, int, int);
static void cursor_position_callback(GLFWwindow*, double, double);
static void handle_input();
static void scroll_callback(GLFWwindow*, double, double);

namespace det {
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
}