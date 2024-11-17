
#include <GL/glew.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <chrono>

#include <imgui_internal.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <Editor.h>
#include <Gui/Renderer.h>
#include <Gui/RendererOptions.h>
#include <Renderer/OBJModel.h>
#include <Structs/SlyLevelFile.h>
#include <Utility/dbgprint.h>

static Editor& editor = Editor::the();

static void handle_input()
{
    editor.handle_input();
}

static void scroll_callback(GLFWwindow*, double xoff, double yoff)
{
    editor.scroll_callback(xoff, yoff);
}

void mouse_button_callback(GLFWwindow*, int button, int action, int mods)
{
    editor.mouse_button_callback(button, action, mods);
}

static void key_callback(GLFWwindow*, int key, int scancode, int action, int mods)
{
    editor.key_callback(key, scancode, action, mods);
}

static void error_callback([[maybe_unused]] int id, const char* desc)
{
    dbgprint("%s\n", desc);
}

static void size_callback(GLFWwindow*, int width, int height)
{
    editor.size_callback(width, height);
}

static void cursor_position_callback(GLFWwindow*, double x, double y)
{
    editor.cursor_position_callback(x, y);
}

void
message_callback([[maybe_unused]] GLenum source,
                [[maybe_unused]] GLenum type,
                [[maybe_unused]] GLuint id,
                [[maybe_unused]] GLenum severity,
                [[maybe_unused]] GLsizei length,
                [[maybe_unused]] const GLchar* message,
                [[maybe_unused]] const void* userParam)
{
    dbgprint("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
             ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
             type, severity, message );
}

int main()
{

    glfwSetErrorCallback(error_callback);
    //glfwWindowHint(GLFW_SAMPLES, 4);

    if (!glfwInit()) {
        dbgprint("ERROR INIT WINDOW");
    }

    //Create the OpenGL context window
    editor.create_window();

    auto* window = editor.window();

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetFramebufferSizeCallback(window, size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);

    glewInit();

    // if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    //     dbgprint("ERROR INIT GLAD\n");
    // }

#ifndef NDEBUG
    // During init, enable debug output
    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( message_callback, 0 );
#endif

    //Init the Editor after establishing a container and linking opengl
    editor.init();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glDisable(GL_CULL_FACE);

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glFrontFace(GL_CCW);

    //For Wireframe:
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glLineWidth(1.0f);

    SingleColoredWorldObject::shader().init();
    SingleColoredSlyWorldObject::shader().init();


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    //io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui_ImplGlfw_InitForOpenGL(editor.window(), true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();

    editor.imgui_init();

    glfwSwapInterval(1);
    auto last_time = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(editor.window()))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        const auto last = last_time;
        float delta_time = ((last_time = std::chrono::high_resolution_clock::now()) - last).count() / 1000000.0f;
        editor.frame(delta_time);
        handle_input();
        editor.render();

        ImGui::Render();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(editor.window());
        glfwPollEvents();
    }
}
