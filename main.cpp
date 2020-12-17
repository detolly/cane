
#include "main.h"
#include <SlyLevelFile.h>
#include <chrono>
#include <lodepng.h>

static int g_width = 1920, g_height = 1080;
static float g_pitch, g_yaw;
constexpr float g_fov = glm::radians(100.f);
GLFWwindow* g_window;

Camera g_camera;

float move_speed = 0.01f;
static float g_delta_time{ 0.0f };

int main(int argc, char* argv[]) {

#ifdef NDEBUG
	CloseHandle(GetStdHandle((DWORD)stdin));
	CloseHandle(GetStdHandle((DWORD)stderr));
	CloseHandle(GetStdHandle((DWORD)stdout));
	PostMessage(GetConsoleWindow(), WM_CLOSE, 0, 0);
	FreeConsole();
#endif

	glfwSetErrorCallback(error_callback);
	//glfwWindowHint(GLFW_SAMPLES, 4);

	if (!glfwInit()) {
		det::dbgprint("ERROR INIT WINDOW");
	}

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	g_window = glfwCreateWindow(g_width, g_height, "OpenGL Testing", NULL, NULL);
	glfwSetWindowTitle(g_window, "cane");

	if (!g_window) {
		glfwTerminate();
		det::dbgprint("ERROR CREATE WINDOW");
	}

	glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetKeyCallback(g_window, key_callback);
	glfwSetCursorPosCallback(g_window, cursor_position_callback);
	glfwSetFramebufferSizeCallback(g_window, size_callback);
	glfwSetScrollCallback(g_window, scroll_callback);

	glfwMakeContextCurrent(g_window);
	glfwSetKeyCallback(g_window, key_callback);

	if (int initcode = glewInit(); initcode != GLEW_OK) {
		det::dbgprint("ERROR INIT GLEW\n%s\n", glewGetErrorString(initcode));
		det::dbgprint("ERROR INIT GLEW\n%s\n", glewGetErrorString(initcode));
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const float aspect = (float)g_width / (float)g_height;
	glm::mat4 g_projection = glm::perspective(g_fov, aspect, 0.2f, 650.f);

	Shader::init_shader(SingleColoredWorldObject::shader());
	Shader::init_shader(SingleColoredSlyWorldObject::shader());

	//Cube cube;
	//Cube cube2;
	//Cube cube3;
	//Cube xCube;
	//Cube yCube;
	//Cube zCube;

	//OBJModel obj("obj.obj");

	//xCube.game_object().set_location({ 0.0f, 5.0f, 0.0f });
	//yCube.game_object().set_location({ 0.0f, 5.0f, 0.0f });
	//zCube.game_object().set_location({ 0.0f, 5.0f, 0.0f });
	//
	//xCube.game_object().set_scale({ 2.0f, 0.1f, 0.1f });
	//yCube.game_object().set_scale({ 0.1f, 2.0f, 0.1f });
	//zCube.game_object().set_scale({ 0.1f, 0.1f, 2.0f });
	//
	//cube.game_object().set_location({ 0.0f, 0.0f, 5.0f });
	//cube2.game_object().set_location({ 5.0f, 0.0f, 0.0f });
	//
	//cube.game_object().set_scale({1.0f, 1.0f, 0.5f});
	//cube2.game_object().set_scale({ 0.5f, 1.0f, 1.0f });
	//
	//cube3.game_object().set_location({ 0.0f, -1.0f, 0.0f });
	//
	//cube3.game_object().set_scale({ 10.0f, 0.1f, 10.0f });

	//obj.game_object().set_location({ 10.0f, 10.0f, 10.0f });
	//obj.game_object().set_scale({5.0f, 5.0f, 5.0f});

	g_camera.set_location({ 0.0f, 2.0f, 0.0f });

	SlyLevelFile level_file("level.bin");
	
	//int index = 0;
	//int num = t.size();
	//for(int i = index; i < index+num; i++)
	//	lodepng::encode("textures/texture " + std::to_string(i) + ".png", t[i].bitmap().data(), t[i].width(), t[i].height());

	RenderedWorldObject* objects[] = {
		//&xCube, &yCube, &zCube,
		//&cube, &cube2, &cube3,
		//&obj,
		&level_file
	};

	glfwSwapInterval(1);
	auto last_time = std::chrono::high_resolution_clock::now();
	while (!glfwWindowShouldClose(g_window))
	{
		auto last = last_time;
		g_delta_time = ((last_time = std::chrono::high_resolution_clock::now()) - last).count()/1000000.0f;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.4f, 0.6f, 1.0f);
		handle_input();

		for (RenderedWorldObject* object : objects)
			object->render(g_camera, g_projection);

		glfwSwapBuffers(g_window);
		glfwPollEvents();
	}

}

static void handle_input() {
	if (glfwGetKey(g_window, GLFW_KEY_W) > 0) {
		glm::vec3 rot = g_camera.forward_xy();
		g_camera.set_location(g_camera.location() + (rot * move_speed * g_delta_time));
		det::dbgprint("%s\n", glm::to_string(g_camera.location()).c_str());
	}
	if (glfwGetKey(g_window, GLFW_KEY_A) > 0) {
		glm::vec3 left = g_camera.left();
		g_camera.set_location(g_camera.location() + (left * move_speed * g_delta_time));
	}
	if (glfwGetKey(g_window, GLFW_KEY_S) > 0) {
		glm::vec3 back = g_camera.back_xy();
		g_camera.set_location(g_camera.location() + (back * move_speed * g_delta_time));
	}
	if (glfwGetKey(g_window, GLFW_KEY_D) > 0) {
		glm::vec3 right = g_camera.right();
		g_camera.set_location(g_camera.location() + (right * move_speed * g_delta_time));
	}
	if (glfwGetKey(g_window, GLFW_KEY_SPACE) > 0) {
		glm::vec3 up = g_camera.up();
		g_camera.set_location(g_camera.location() + (up * move_speed * g_delta_time));
	}
	if (glfwGetKey(g_window, GLFW_KEY_LEFT_SHIFT) > 0) {
		glm::vec3 up = g_camera.up();
		g_camera.set_location(g_camera.location() - (up * move_speed * g_delta_time));
	}
}

static int current_cursor_mode{ GLFW_CURSOR_DISABLED };

static void scroll_callback(GLFWwindow* window, double xoff, double yoff)
{
	move_speed += (((float)yoff)-(yoff < 0.0f ? -0.995f : 0.995f));
	move_speed = std::min(std::max(move_speed, 0.0f), .5f);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		if (current_cursor_mode != GLFW_CURSOR_DISABLED) {
			glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			current_cursor_mode = GLFW_CURSOR_DISABLED;
		}
		else {
			glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			current_cursor_mode = GLFW_CURSOR_NORMAL;
		}//glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

static void error_callback(int id, const char* desc) {
	det::dbgprint("%s\n", desc);
}

static void size_callback(GLFWwindow* window, int width, int height) {
	g_width = width;
	g_height = height;
	const float aspect = (float)g_width / (float)g_height;
	//g_projection = glm::perspective(g_fov, aspect, 0.1f, 100.f);
	glViewport(0, 0, g_width, g_height);
}



static void cursor_position_callback(GLFWwindow*, double x, double y) {
	static double lastX, lastY;
	if (current_cursor_mode == GLFW_CURSOR_DISABLED) {
		double x_diff = lastX - x;
		double y_diff = lastY - y;
		g_camera.set_yaw_pitch(g_camera.yaw() - x_diff * 0.1, g_camera.pitch() - y_diff * 0.1);
		//det::dbgprint("%f %f\n", g_camera.yaw(), g_camera.pitch());
	}
	lastX = x; lastY = y;
}