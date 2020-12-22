
#include "main.h"

static GLFWwindow* g_window;
static float g_delta_time{ 0.0f };
static float g_width = 1600;
static float g_height = 900;

static ImGuiIO* io;

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
		dbgprint("ERROR INIT WINDOW");
	}

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	g_window = glfwCreateWindow(g_width, g_height, "OpenGL Testing", NULL, NULL);
	glfwSetWindowTitle(g_window, "cane");

	if (!g_window) {
		glfwTerminate();
		dbgprint("ERROR CREATE WINDOW");
	}

	//glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	current_cursor_mode = GLFW_CURSOR_NORMAL;

	glfwSetKeyCallback(g_window, key_callback);
	glfwSetCursorPosCallback(g_window, cursor_position_callback);
	glfwSetFramebufferSizeCallback(g_window, size_callback);
	glfwSetScrollCallback(g_window, scroll_callback);
	glfwSetMouseButtonCallback(g_window, mouse_button_callback);

	glfwMakeContextCurrent(g_window);
	glfwSetKeyCallback(g_window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		dbgprint("ERROR INIT GLAD\n");
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader::init_shader(SingleColoredWorldObject::shader());
	Shader::init_shader(SingleColoredSlyWorldObject::shader());

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO();
	//io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	ImGui_ImplGlfw_InitForOpenGL(g_window, true);
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

	Editor::init();
	Editor::the().open("level.bin");
	
	//int index = 0;
	//int num = t.size();
	//for(int i = index; i < index+num; i++)
	//	lodepng::encode("textures/texture " + std::to_string(i) + ".png", t[i].bitmap().data(), t[i].width(), t[i].height());

	glfwSwapInterval(1);
	auto last_time = std::chrono::high_resolution_clock::now();
	while (!glfwWindowShouldClose(g_window))
	{
		auto last = last_time;
		g_delta_time = ((last_time = std::chrono::high_resolution_clock::now()) - last).count() / 1000000.0f;

		handle_input();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		using wf = ImGuiWindowFlags_;
		ImGui::SetNextWindowSize({ (float)g_width, (float)g_height }, ImGuiCond_::ImGuiCond_Always);
		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::Begin("root", nullptr, wf::ImGuiWindowFlags_NoCollapse | wf::ImGuiWindowFlags_NoTitleBar | wf::ImGuiWindowFlags_NoMove | wf::ImGuiWindowFlags_NoResize | wf::ImGuiWindowFlags_NoBringToFrontOnFocus | wf::ImGuiWindowFlags_NoInputs | wf::ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Windows"))
			{
				ImGui::MenuItem("Renderer", nullptr, &config::the().windows.renderer);
				ImGui::MenuItem("Debug Information", nullptr, &config::the().windows.debug_information);
				ImGui::MenuItem("Model Browser", nullptr, &config::the().windows.model_browser);
				ImGui::MenuItem("Model Viewer", nullptr, &config::the().windows.model_viewer);
				ImGui::MenuItem("Examples", nullptr, &config::the().windows.examples);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Options"))
			{
				if (ImGui::MenuItem("Preferences")) {

				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_NoCloseButton;
		ImGuiID dockspaceID = ImGui::GetID("dock");
		//ImGui::SetNextWindowSize(ImGui::GetContentRegionAvail());
		ImGui::SetNextWindowSize({ (float)g_width, (float)g_height });
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);
		if (config::the().windows.debug_information) {
			//ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
			Editor::the().debug_window()->render();
		}
		if (config::the().windows.renderer) {
			//ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
			Editor::the().renderer()->render();
		}
		if (config::the().windows.model_browser) {
			//ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
			Editor::the().model_browser()->render();
		}
		if (config::the().windows.model_viewer) {
			Editor::the().model_viewer()->render();
		}
		if (config::the().windows.examples) {
			ImGui::ShowDemoWindow(&config::the().windows.examples);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, g_width, g_height);

		ImGui::End();

		ImGui::Render();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(g_window);
		glfwPollEvents();
	}

}

static void handle_input() {
	if (current_cursor_mode != GLFW_CURSOR_NORMAL) {
		Editor::the().renderer()->handle_input(g_window, g_delta_time);
	}
}

static void scroll_callback(GLFWwindow* window, double xoff, double yoff)
{
	if (current_cursor_mode != GLFW_CURSOR_NORMAL) {
		config::the().renderer.move_speed += (((float)yoff) - (yoff < 0.0f ? -0.995f : 0.995f));
		config::the().renderer.move_speed = std::min(std::max(config::the().renderer.move_speed, 0.0f), .2f);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		Editor::the().set_can_resize(false);
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		Editor::the().set_can_resize(true);
	//TODO: Ctrl+click should select multiple objects for exporting into one mesh / moving and so on
	double mouse_x, mouse_y;
	glfwGetCursorPos(window, &mouse_x, &mouse_y);
	const auto location = Editor::the().renderer()->render_location();
	const auto render_size = Editor::the().renderer()->render_size();
	static bool last_left_click_was_inside_renderer{false};
	if (config::the().windows.renderer && last_left_click_was_inside_renderer && button == GLFW_MOUSE_BUTTON_LEFT && current_cursor_mode == GLFW_CURSOR_NORMAL && action == GLFW_RELEASE
		&& mouse_x > location.x && mouse_y > location.y
		&& mouse_x < location.x + render_size.x && mouse_y < location.y + render_size.y) {
		Editor::the().renderer()->select(mouse_x - location.x, mouse_y - location.y, mods & GLFW_MOD_CONTROL);
	}
	if (config::the().windows.renderer && button == GLFW_MOUSE_BUTTON_LEFT && current_cursor_mode == GLFW_CURSOR_NORMAL && action == GLFW_PRESS && mouse_x > location.x && mouse_y > location.y
		&& mouse_x < location.x + render_size.x && mouse_y < location.y + render_size.y)
		last_left_click_was_inside_renderer = true;
	else
		last_left_click_was_inside_renderer = false;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_V && action == GLFW_PRESS) {
		if (current_cursor_mode != GLFW_CURSOR_DISABLED) {
			glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			current_cursor_mode = GLFW_CURSOR_DISABLED;
			ImGui::SetMouseCursor(ImGuiMouseCursor_None);
			io->ConfigFlags |= ImGuiConfigFlags_NoMouse;
		}
		else {
			glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			current_cursor_mode = GLFW_CURSOR_NORMAL;
			io->ConfigFlags = io->ConfigFlags & ~(1<<4);
		}
	}
}

static void error_callback(int id, const char* desc) {
	dbgprint("%s\n", desc);
}

static void size_callback(GLFWwindow* window, int width, int height) {
	Editor::the().size_callback(width, height);
	g_width = width;
	g_height = height;
}

static void cursor_position_callback(GLFWwindow*, double x, double y) {
	static double lastX, lastY;
	if (current_cursor_mode == GLFW_CURSOR_DISABLED) {
		double x_diff = x - lastX;
		double y_diff = y - lastY;
		//todo more abstraction
		Editor::the().renderer()->camera().set_yaw_pitch(
			Editor::the().renderer()->camera().yaw() + x_diff * 0.1,
			Editor::the().renderer()->camera().pitch() - y_diff * 0.1
		);
	}
	lastX = x; lastY = y;
}