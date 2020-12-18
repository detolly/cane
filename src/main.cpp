
#include "main.h"

static int g_width = 1920, g_height = 1080;
static float g_pitch, g_yaw;
constexpr float g_fov = glm::radians(100.f);
static GLFWwindow* g_window;

static Camera g_camera;

float move_speed = 0.01f;
static float g_delta_time{ 0.0f };

static SlyLevelFile* g_level_file{nullptr};
static glm::mat4 g_projection;
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

	glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

	const float aspect = (float)g_width / (float)g_height;
	g_projection = glm::perspective(g_fov, aspect, 0.2f, 650.f);

	Shader::init_shader(SingleColoredWorldObject::shader());
	Shader::init_shader(SingleColoredSlyWorldObject::shader());

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(g_window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
	ImGui::StyleColorsDark();

	//Cube cube;
	//Cube cube2;
	//Cube cube3;
	//Cube xCube;
	//Cube xxCube;
	//Cube xxxCube;

	//OBJModel obj("obj.obj");

	//xCube.game_object().set_location({ 1.0f, 5.0f, 1.0f });
	//xxCube.game_object().set_location({ -1.0f, 5.0f, 2.0f });
	//xxxCube.game_object().set_location({ 1.0f, 5.0f, 3.0f });
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

	g_camera.set_location({ 15.0f, 2.0f, 0.0f });

	SlyLevelFile level_file("level.bin");
	g_level_file = &level_file;
	
	//int index = 0;
	//int num = t.size();
	//for(int i = index; i < index+num; i++)
	//	lodepng::encode("textures/texture " + std::to_string(i) + ".png", t[i].bitmap().data(), t[i].width(), t[i].height());

	RenderedWorldObject* objects[] = {
		//&xCube, &xxCube, &xxxCube,
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

		if (current_cursor_mode == GLFW_CURSOR_NORMAL)
			render_gui();

		glfwSwapBuffers(g_window);
		glfwPollEvents();
	}

}

static void render_gui() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	float width = ImGui::GetWindowWidth();
	float height = ImGui::GetWindowHeight();
	{
		ImGui::Begin("Debug Information");
		ImGui::Text("Selected Mesh: ");
		if (currently_selected_mesh > -1) {
			auto& mesh = g_level_file->meshes()[currently_selected_mesh];
			ImGui::Text("Flags: %08x", mesh.mesh_data.flags);
			ImGui::DragFloat3("XYZ", (float*)&mesh.game_object().raw_location(), 0.1f, -100.0f, 100.0f, "%.3f", 1.0f);
			if (~mesh.mesh_data.flags & 1) {
				ImGui::Text("Mesh Header:");
				auto& na = mesh.mesh_data.not_flags_and_1;
				ImGui::Text("Mesh Count: %d\nUK1: %d | UK2: %d", na.mesh_hdr.mesh_count, na.mesh_hdr.unknown_0x00, na.mesh_hdr.unknown_0x04);
				ImGui::Text("SZME Data: ");
				for (int i = 0; i < na.szme_data.size(); i++) {
					ImGui::Text("uc1 %03d\tuc2 %03d\tuc3 %03d\tuc4 %03d\tuc5 %03d", na.szme_data[i].unk_count1, na.szme_data[i].unk_count2, na.szme_data[i].unk_count3, na.szme_data[i].unk_count4, na.szme_data[i].unk_count5);
				}
			}
			mesh.game_object().set_should_recalculate_model_matrix(true);
		}
		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static void handle_input() {
	if (current_cursor_mode != GLFW_CURSOR_NORMAL) {
		if (glfwGetKey(g_window, GLFW_KEY_W) > 0) {
			glm::vec3 rot = g_camera.forward_xy();
			g_camera.set_location(g_camera.location() + (rot * move_speed * g_delta_time));
			//dbgprint("%s\n", glm::to_string(g_camera.location()).c_str());
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
}

static void scroll_callback(GLFWwindow* window, double xoff, double yoff)
{
	if (current_cursor_mode != GLFW_CURSOR_NORMAL) {
		move_speed += (((float)yoff) - (yoff < 0.0f ? -0.995f : 0.995f));
		move_speed = std::min(std::max(move_speed, 0.0f), .5f);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	//TODO: Ctrl+click should select multiple objects for exporting into one mesh / moving and so on
	if (!io->WantCaptureMouse && button == GLFW_MOUSE_BUTTON_LEFT && current_cursor_mode == GLFW_CURSOR_NORMAL && action == GLFW_RELEASE) {
		double mouse_x, mouse_y;
		glfwGetCursorPos(window, &mouse_x, &mouse_y);
		float x = (2.0f * mouse_x) / g_width - 1.0f;
		float y = 1.0f - (2.0f * mouse_y) / g_height;
		float z = 1.0f;
		//dbgprint("screen coords: %f %f\n", x, y);
		glm::vec3 ray_nds = glm::vec3(x, y, z);
		glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);
		glm::vec4 ray_eye = glm::inverse(g_projection) * ray_clip;
		ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
		auto temp = glm::normalize(glm::inverse(g_camera.view()) * ray_eye);
		glm::vec3 ray = glm::vec3(temp.x, temp.y, temp.z);
		//dbgprint("%f %f %f\n", ray.x, ray.y, ray.z);
		std::vector<SlyMesh>& meshes = g_level_file->meshes();
		int mesh = -1;
		bool has_found = false;
		float lowest_distance = 10000000.0f;
		glm::vec3 intersection_point;
		for (int i = 0; i < meshes.size(); i++) {
			if (~meshes[i].mesh_data.flags & 1)
				for (int j = 0; j < meshes[i].mesh_data.not_flags_and_1.mesh_hdr.mesh_count; j++) {
					const auto& triangles = meshes[i].mesh_data.not_flags_and_1.vertex_data[j].index_hdr.triangle_data;
					const auto& vertices = meshes[i].mesh_data.not_flags_and_1.vertex_data[j].vertices;
					for (int tri = 0; tri < triangles.size(); tri += 3) {
						if (ray_intersects_triangle(
							g_camera.location(),
							ray,
							vertices[triangles[tri]].to_glm_vec3() + meshes[i].game_object().location(),
							vertices[triangles[tri+1]].to_glm_vec3() + meshes[i].game_object().location(),
							vertices[triangles[tri+2]].to_glm_vec3() + meshes[i].game_object().location(),
							intersection_point
						)) {
							float len = glm::length(g_camera.location() - intersection_point);
							if (len < lowest_distance) {
								has_found = true;
								mesh = i;
								lowest_distance = len;
							}
						}
					}
				}
		}
		if (!has_found) {
			currently_selected_mesh = -1;
		}
		else {
			currently_selected_mesh = mesh;
		}
	}
}

/* https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm thanks wikipedia<3*/
bool ray_intersects_triangle(const glm::vec3& ray_origin,
	const glm::vec3& ray_vector,
	const glm::vec3& vertex0, const glm::vec3& vertex1, const glm::vec3& vertex2, 
	glm::vec3& outIntersectionPoint)
{
	const float EPSILON = 0.0000001;
	glm::vec3 edge1, edge2, h, s, q;
	float a, f, u, v;
	edge1 = vertex1 - vertex0;
	edge2 = vertex2 - vertex0;
	h = glm::cross(ray_vector, edge2);
	a = glm::dot(edge1, h);
	if (a > -EPSILON && a < EPSILON)
		return false;    // This ray is parallel to this triangle.
	f = 1.0 / a;
	s = ray_origin - vertex0;
	u = f * glm::dot(s, h);
	if (u < 0.0 || u > 1.0)
		return false;
	q = glm::cross(s, edge1);
	v = f * glm::dot(ray_vector, q);
	if (v < 0.0 || u + v > 1.0)
		return false;
	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = f * glm::dot(edge2, q);
	if (t > EPSILON) // ray intersection
	{
		outIntersectionPoint = ray_origin + ray_vector * t;
		return true;
	}
	else // This means that there is a line intersection but not a ray intersection.
		return false;
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
	//dbgprint("%s\n", desc);
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
		//det:://dbgprint("%f %f\n", g_camera.yaw(), g_camera.pitch());
	}
	lastX = x; lastY = y;
}