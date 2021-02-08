
#include "main.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <chrono>

#include <Gui/RendererOptions.h>
#include <Gui/Renderer.h>

#include <imgui_internal.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <Utility/dbgprint.h>
#include <Renderer/OBJModel.h>
#include <Structs/SlyLevelFile.h>
#include <Editor.h>

int main(int argc, char* argv[]) {
#ifdef WIN32
#ifdef NDEBUG
	CloseHandle(GetStdHandle((DWORD)stdin));
	CloseHandle(GetStdHandle((DWORD)stderr));
	CloseHandle(GetStdHandle((DWORD)stdout));
	PostMessage(GetConsoleWindow(), WM_CLOSE, 0, 0);
	FreeConsole();
#endif
#endif

	glfwSetErrorCallback(error_callback);
	//glfwWindowHint(GLFW_SAMPLES, 4);

	if (!glfwInit()) {
		dbgprint("%s", "ERROR INIT WINDOW");
	}

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	//Create the OpenGL context window
	Editor::the().create_window();

    glfwSetKeyCallback(Editor::the().window(), key_callback);
    glfwSetCursorPosCallback(Editor::the().window(), cursor_position_callback);
    glfwSetFramebufferSizeCallback(Editor::the().window(), size_callback);
    glfwSetScrollCallback(Editor::the().window(), scroll_callback);
    glfwSetMouseButtonCallback(Editor::the().window(), mouse_button_callback);
    glfwSetKeyCallback(Editor::the().window(), key_callback);
    glfwMakeContextCurrent(Editor::the().window());

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		dbgprint("%s", "ERROR INIT GLAD\n");
	}

	//Init the Editor after establishing a container
	Editor::the().init();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glFrontFace(GL_CW);

	Shader::init_shader(SingleColoredWorldObject::shader());
	Shader::init_shader(SingleColoredSlyWorldObject::shader());


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	auto* io = &ImGui::GetIO();
	//io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui_ImplGlfw_InitForOpenGL(Editor::the().window(), true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
	ImGui::StyleColorsDark();

	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io->ConfigDockingWithShift = false;
	//io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGuiStyle& style = ImGui::GetStyle();
	style.FrameRounding = 0.0f;
	style.GrabRounding = 0.0f;
	style.ChildRounding = 0.0f;
	style.PopupRounding = 0.0f;
	style.WindowRounding = 0.0f;
	style.ScrollbarRounding = 0.0f;
	style.TabRounding = 0.0f;


	glfwSwapInterval(1);
	auto last_time = std::chrono::high_resolution_clock::now();
	while (!glfwWindowShouldClose(Editor::the().window()))
	{
		auto last = last_time;
		float delta_time = ((last_time = std::chrono::high_resolution_clock::now()) - last).count() / 1000000.0f;

		Editor::the().frame(delta_time);

		handle_input();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		Editor::the().render();

		ImGui::Render();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(Editor::the().window());
		glfwPollEvents();
	}

}

static void handle_input()
{
    Editor::the().handle_input();
}

static void scroll_callback(GLFWwindow* window, double xoff, double yoff)
{
	Editor::the().scroll_callback(xoff, yoff);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    Editor::the().mouse_button_callback(button, action, mods);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Editor::the().key_callback(key, scancode, action, mods);
}

static void error_callback(int id, const char* desc) {
	dbgprint("%s\n", desc);
}

static void size_callback(GLFWwindow* window, int width, int height) {
	Editor::the().size_callback(width, height);
}

static void cursor_position_callback(GLFWwindow*, double x, double y) {
    Editor::the().cursor_position_callback(x, y);
}