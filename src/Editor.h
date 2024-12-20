#pragma once

#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <imgui.h>

#include <Utility/Singleton.h>

class SlyWorld;
class Renderer;
class DebugInformation;
class ModelBrowser;
class ModelViewer;
class RendererOptions;

class Editor final : public Singleton<Editor> {
    friend class Singleton<Editor>;
private:
    Editor() = default;
    ~Editor() = default;
public:

    void init();
    void imgui_init();
    void create_window();

    std::unique_ptr<SlyWorld>& level_file() { return m_level; }
    const std::unique_ptr<SlyWorld>& level_file() const { return m_level; }
    bool has_file_loaded() const { return m_level != nullptr; }

    void set_can_resize(bool value) { m_can_resize = value; }
    bool can_resize() const { return m_can_resize; }

    void open(const char* file);
    void close();

    void render();
    void frame(float delta_time) { m_delta_time = delta_time; };

    float width() const { return m_width; }
    float height() const { return m_height; }
    float delta_time() const { return m_delta_time; }
    int cursor_mode() const { return m_cursor_mode; }

    ImGuiIO* io() const { return m_io; }
    GLFWwindow* window() const { return m_window; }
    void set_title(const char* title) const { glfwSetWindowTitle(window(), title); }

    void set_cursor_mode(int cursor_mode) { m_cursor_mode = cursor_mode; glfwSetInputMode(window(), GLFW_CURSOR, cursor_mode); }

private:
    void set_width(float w) { m_width = w; /*TODO: Callback*/ }
    void set_height(float h) { m_height = h; }
    void set_window(GLFWwindow* window) { m_window = window; }

public:

    void key_callback(int, int, int, int);
    void error_callback(int, const char* desc);
    void size_callback(int, int);
    void cursor_position_callback(double, double);
    void handle_input();
    void scroll_callback(double, double);
    void mouse_button_callback(int, int, int);

private:

    float m_delta_time { 0.0f };
    float m_width  { 1600 };
    float m_height { 900 };

    ImGuiIO* m_io;

    GLFWwindow* m_window;
    int m_cursor_mode { GLFW_CURSOR_DISABLED };

    std::unique_ptr<SlyWorld> m_level { nullptr };
    bool m_can_resize { true };

};

