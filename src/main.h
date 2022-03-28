#pragma once

struct GLFWwindow;

static void key_callback(GLFWwindow*, int, int, int, int);
static void error_callback(int, const char* desc);
static void size_callback(GLFWwindow*, int, int);
static void cursor_position_callback(GLFWwindow*, double, double);
static void handle_input();
static void scroll_callback(GLFWwindow*, double, double);
static void mouse_button_callback(GLFWwindow*, int, int, int);

