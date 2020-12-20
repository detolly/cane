
#include "main.h"

static int g_width = 1600, g_height = 900;
static bool can_resize = true;
static ImVec2 g_render_location;
static ImVec2 g_render_size;

static float g_pitch, g_yaw;
constexpr float g_fov = glm::radians(100.f);
static GLFWwindow* g_window;

static Camera g_camera;

float move_speed = 0.01f;
static float g_delta_time{ 0.0f };

static SlyLevelFile* g_level_file{nullptr};
static glm::mat4 g_projection;
static ImGuiIO* io;

static GLuint g_depthrenderbuffer;
static GLuint g_render_texture;
static GLuint g_renderer_fbo;

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

	const float aspect = (float)g_width / (float)g_height;
	g_projection = glm::perspective(g_fov, aspect, 0.2f, 650.f);

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
	
	//Cube cube;
	//Cube cube2;
	//Cube cube3;
	Cube xCube;
	Cube xxCube;
	Cube xxxCube;

	//OBJModel obj("obj.obj");

	xCube.game_object().set_location({ 1.0f, 5.0f, 1.0f });
	xxCube.game_object().set_location({ -1.0f, 5.0f, 2.0f });
	xxxCube.game_object().set_location({ 1.0f, 5.0f, 3.0f });
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

	g_objects = {
		&xCube, &xxCube, &xxxCube,
		//&cube, &cube2, &cube3,
		//&obj,
		&level_file
	};

	glfwSwapInterval(1);
	auto last_time = std::chrono::high_resolution_clock::now();
	while (!glfwWindowShouldClose(g_window))
	{
		auto last = last_time;
		g_delta_time = ((last_time = std::chrono::high_resolution_clock::now()) - last).count() / 1000000.0f;

		handle_input();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		if (g_renderer_fbo) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		using wf = ImGuiWindowFlags_;
		ImGui::SetNextWindowSize({ (float)g_width, (float)g_height });
		ImGui::Begin("root", nullptr, wf::ImGuiWindowFlags_NoCollapse | wf::ImGuiWindowFlags_NoTitleBar | wf::ImGuiWindowFlags_NoMove | wf::ImGuiWindowFlags_NoResize | wf::ImGuiWindowFlags_NoBringToFrontOnFocus | wf::ImGuiWindowFlags_NoInputs | wf::ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Windows"))
			{
				ImGui::MenuItem("Renderer", nullptr, &config::the().windows.renderer);
				ImGui::MenuItem("Debug Information", nullptr, &config::the().windows.debug_information);
				ImGui::MenuItem("Mesh Browser", nullptr, &config::the().windows.mesh_browser);
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
		ImGuiID dockspaceID = ImGui::GetID("root");
		//ImGui::SetNextWindowSize(ImGui::GetContentRegionAvail());
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspaceFlags);

		render_gui();
		render_renderer();
		render_mesh_browser();

		ImGui::End();

		ImGui::Render();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(g_window);
		glfwPollEvents();
	}

}

static void make_render_texture(int width, int height) {

	if (g_render_texture != 0) {
		glDeleteTextures(1, &g_render_texture);
	}
	if (g_renderer_fbo != 0) {
		glDeleteFramebuffers(1, &g_renderer_fbo);
	}
	if (g_depthrenderbuffer != 0) {
		glDeleteRenderbuffers(1, &g_depthrenderbuffer);
	}


	static int msaa = 4;

	glGenFramebuffers(1, &g_renderer_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, g_renderer_fbo);
	glViewport(0, 0, width, height);
	
	glGenRenderbuffers(1, &g_depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, g_depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	
	glGenTextures(1, &g_render_texture);
	glBindTexture(GL_TEXTURE_2D, g_render_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_render_texture, 0);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, g_depthrenderbuffer);


	const auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (result) {
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:				dbgprint("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:		dbgprint("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:				dbgprint("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS\n"); break;
	case GL_FRAMEBUFFER_UNSUPPORTED:						dbgprint("GL_FRAMEBUFFER_UNSUPPORTED\n"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:				dbgprint("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\n"); break;
	case GL_RENDERBUFFER_SAMPLES:							dbgprint("GL_RENDERBUFFER_SAMPLES\n"); break;
	case GL_FRAMEBUFFER_INCOMPLETE_VIEW_TARGETS_OVR:		dbgprint("GL_FRAMEBUFFER_INCOMPLETE_VIEW_TARGETS_OVR\n"); break;
	case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE:				dbgprint("GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE\n"); break;
	}
	if (result != GL_FRAMEBUFFER_COMPLETE) {
		dbgprint("Framebuffer error: %d\n", glGetError());
	}
}

static void render_mesh_browser() {
	if (ImGui::Begin("Mesh Browser", &config::the().windows.mesh_browser)) {

	}
}

static void render_renderer() {
	using wf = ImGuiWindowFlags_;
	if (ImGui::Begin("Renderer", &config::the().windows.renderer))
	{
		glBindFramebuffer(GL_FRAMEBUFFER, g_renderer_fbo);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.4f, 0.6f, 1.0f);

		for (RenderedWorldObject* object : g_objects)
			object->render(g_camera, g_projection);

		if (currently_selected_mesh != -1) {
			auto& mesh = g_level_file->meshes()[currently_selected_mesh];
			if (~mesh.mesh_data.flags & 1) {
				auto& na = mesh.mesh_data.not_flags_and_1;
				for (int i = 0; i < na.szme_data.size(); i++) {
					na.szme_data[i].render(g_camera, g_projection);
				}
			}
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::BeginMenu("View")) {
				ImGui::MenuItem("Wireframe", nullptr, &config::the().renderer.wireframe);
				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
		g_render_location = ImGui::GetWindowPos();
		const auto csize = ImGui::GetWindowContentRegionMax();

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		g_render_location.x += vMin.x;
		g_render_location.y += vMin.y;

		if (can_resize && (g_render_size.x != csize.x || g_render_size.y != csize.y)) {
			make_render_texture(csize.x, csize.y);
			g_render_size = csize;
		}
		ImGui::Image((ImTextureID)g_render_texture, g_render_size, { 0, 1 }, { 1, 0 } );
		char buf[64];
		sprintf(buf, "%.4f %.4f", g_camera.yaw(), g_camera.pitch());
		const auto yaw_text_size = ImGui::CalcTextSize(buf);
		sprintf(buf, "%.4f %.4f %.4f", g_camera.location().x, g_camera.location().y, g_camera.location().z);
		const auto text_size = ImGui::CalcTextSize(buf);
		ImGui::SetNextWindowSize({ text_size.x + 50.f, 60.0f });
		ImGui::SetNextWindowPos({ g_render_location.x, g_render_location.y});
		ImGui::SetNextWindowBgAlpha(0.5f);
			ImGui::Begin("asd", nullptr, wf::ImGuiWindowFlags_NoTitleBar | wf::ImGuiWindowFlags_NoMove | wf::ImGuiWindowFlags_NoCollapse | wf::ImGuiWindowFlags_NoDocking | wf::ImGuiWindowFlags_NoResize);
			ImGui::BeginChild("Coords", {}, false, wf::ImGuiWindowFlags_NoCollapse | wf::ImGuiWindowFlags_NoMove | wf::ImGuiWindowFlags_NoTitleBar | wf::ImGuiWindowFlags_NoDecoration | wf::ImGuiWindowFlags_NoResize | wf::ImGuiWindowFlags_NoScrollbar | wf::ImGuiWindowFlags_NoDocking);
				ImGui::SameLine((text_size.x + 50.f) / 2.0f - (text_size.x / 2.0f));
				ImGui::Text("%.4f %.4f %.4f", g_camera.location().x, g_camera.location().y, g_camera.location().z);
				ImGui::NewLine();
				ImGui::SameLine((yaw_text_size.x + 50.f) / 2.0f - (yaw_text_size.x / 2.0f));
				ImGui::Text("%.4f %.4f", g_camera.yaw(), g_camera.pitch());
			ImGui::EndChild();
			ImGui::End();
		ImGui::End();
	}
}

static void render_gui() {
	{
		if (ImGui::Begin("Debug Information", &config::the().windows.debug_information)) {
			ImGui::Text("Selected Mesh: ");
			if (currently_selected_mesh > -1) {
				auto& mesh = g_level_file->meshes()[currently_selected_mesh];
				ImGui::DragFloat3("XYZ", (float*)&mesh.game_object().raw_location(), 0.1f, 0.0f, 0.0f, "%.3f", 1.0f);
				if (~mesh.mesh_data.flags & 1) {
					ImGui::Text("Mesh Header:");
					auto& na = mesh.mesh_data.not_flags_and_1;
					ImGui::Text("Unknown 0x00 0x%08x", na.mesh_hdr.unknown_0x00);
					ImGui::Text("Unknown 0x04 0x%04x", na.mesh_hdr.unknown_0x04);
					ImGui::Text("Mesh Count: %d\nUK1: %d | UK2: %d", na.mesh_hdr.mesh_count, na.mesh_hdr.unknown_0x00, na.mesh_hdr.unknown_0x04);
					
					auto& header = mesh.mesh_data.not_flags_and_1.szme_hdr;
					ImGui::Text("SZME Header: ");
					ImGui::Text("Flags: 0x%08x", mesh.mesh_data.flags);
					ImGui::Text("Relevant Flag Information:");
					ImGui::Text(" Flag & 0x2  \t Uint32_t 0x%08x", header.m.unk_0x04);
					ImGui::Text(" Flag & 0x200\t Float %.4f", header.m.unk_float);
					ImGui::Text(" Flag & 0x4  \t Float %.4f", header.m.unk_float2);
					ImGui::Text(" Flag & 0x8  \t Float %.4f", header.m.unk_float3);
					ImGui::Text(" Flag & 0x10 \t Float %.4f", header.m.unk_float4);
					ImGui::Text(" Flag & 0x20 \t Float %.4f", header.m.unk_float5);
					ImGui::Text("~Flag & 0x100\t Float %.4f", header.m.unk_0x14);
					ImGui::Text("~Flag & 0x100\t Vec3 %.4f %.4f %.4f (O position)", header.m.position.x, header.m.position.y, header.m.position.z);
					ImGui::Text("~Flag & 0x100\t unk_0x16_ignore: 0x%08x (%9d)", header.m.unk_0x16_ignore, header.m.unk_0x16_ignore);
					ImGui::Text("~Flag & 0x100\t unk_byte_1A: %03x (%03d)", header.m.unk_0x1A, header.m.unk_0x1A);
					ImGui::Text("~Flag & 0x100\t unk_byte_1B: %03x (%03d)", header.m.unk_0x1B, header.m.unk_0x1B);
					ImGui::Text("~Flag & 0x100\t unk_byte_1C: %03x (%03d)", header.m.unk_0x1C, header.m.unk_0x1C);
					uint16_t* ptr = (uint16_t*)&header.m.unk_0x1A;
					uint16_t* ptr2 = (uint16_t*)&header.m.unk_0x1B;
					ImGui::Text("~Flag & 0x100\t unk_byte_1A1B: %04x (%05d)", *ptr, *ptr);
					ImGui::Text("~Flag & 0x100\t unk_byte_1B1C: %04x (%05d)", *ptr2, *ptr2);
					
					ImGui::Text("SZME Data: ");
					for (int i = 0; i < na.szme_data.size(); i++) {
						char buf[32];
						sprintf(buf, "SZME Data #%d", i);
						if (ImGui::CollapsingHeader(buf)) {
							ImGui::Text("unk_u8_1 %03x (%03d)", na.szme_data[i].unk_u8_1, na.szme_data[i].unk_u8_1);
							ImGui::Text("unk_u8_2 %03x (%03d)", na.szme_data[i].unk_u8_2, na.szme_data[i].unk_u8_2);
							ImGui::Text("UnkVec %.4f %.4f %.4f", na.szme_data[i].unk_vec.x, na.szme_data[i].unk_vec.y, na.szme_data[i].unk_vec.z);
							ImGui::Text("position_count %03d\trotation_count %03d\tuc3 %03d\ttexcoords_count %03d\tlighing_count %03d", na.szme_data[i].position_count, na.szme_data[i].rotation_count, na.szme_data[i].unk_count3, na.szme_data[i].texcoords_count, na.szme_data[i].lighing_count);
							ImGui::Text("Positions information: ");
							for (int j = 0; j < na.szme_data[i].positions.size(); j++) {
								ImGui::Text("%f %f %f", na.szme_data[i].positions[j].x, na.szme_data[i].positions[j].y, na.szme_data[i].positions[j].z);
							}
						}
					}
				}
				mesh.game_object().set_should_recalculate_model_matrix(true);
			}
			ImGui::End();
		}
	}

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
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
		can_resize = false;
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
		can_resize = true;
	//TODO: Ctrl+click should select multiple objects for exporting into one mesh / moving and so on
	double mouse_x, mouse_y;
	glfwGetCursorPos(window, &mouse_x, &mouse_y);
	if (button == GLFW_MOUSE_BUTTON_LEFT && current_cursor_mode == GLFW_CURSOR_NORMAL && action == GLFW_RELEASE
		&& mouse_x > g_render_location.x && mouse_y > g_render_location.y
		&& mouse_x < g_render_location.x + g_render_size.x && mouse_y < g_render_location.y + g_render_size.y) {
		glm::vec3 ray = clickray(mouse_x - g_render_location.x, mouse_y - g_render_location.y, g_render_size.x, g_render_size.y, g_projection, g_camera);
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
							vertices[triangles[tri]].pos + meshes[i].game_object().location(),
							vertices[triangles[tri+1]].pos + meshes[i].game_object().location(),
							vertices[triangles[tri+2]].pos + meshes[i].game_object().location(),
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
		currently_selected_mesh = has_found ? mesh : -1;
	}
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
	//dbgprint("%s\n", desc);
}

static void size_callback(GLFWwindow* window, int width, int height) {
	g_width = width;
	g_height = height;
	const float aspect = (float)g_width / (float)g_height;
	g_projection = glm::perspective(g_fov, aspect, 0.2f, 650.0f);
	glViewport(0, 0, g_width, g_height);
}

static void cursor_position_callback(GLFWwindow*, double x, double y) {
	static double lastX, lastY;
	if (current_cursor_mode == GLFW_CURSOR_DISABLED) {
		double x_diff = x - lastX;
		double y_diff = y - lastY;
		g_camera.set_yaw_pitch(g_camera.yaw() + x_diff * 0.1, g_camera.pitch() - y_diff * 0.1);
		//det:://dbgprint("%f %f\n", g_camera.yaw(), g_camera.pitch());
	}
	lastX = x; lastY = y;
}