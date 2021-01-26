#include <Gui/Renderer.h>
#include <Editor.h>
#include <glm/mat4x4.hpp>
#include <Structs/SlyLevelFile.h>

void Renderer::render()
{
	auto& editor = Editor::the();
	using wf = ImGuiWindowFlags_;
	ImGui::Begin("Renderer", &config::the().windows.renderer);
	ImVec2 vMin = ImGui::GetWindowContentRegionMin();
	auto csize = ImGui::GetWindowContentRegionMax();
	csize.x -= vMin.x;
	csize.y -= vMin.y;
	if (!is_allocated()) {
		m_render_size = csize;
		resize_buffer(csize.x, csize.y);
	}
	else if ((m_render_size.x != csize.x || m_render_size.y != csize.y) && Editor::the().can_resize()) {
		m_render_size = csize;
		resize_buffer(csize.x, csize.y);
		set_should_recalculate_projection(true);
	}

	m_render_location = ImGui::GetWindowPos();
	m_render_location.x += vMin.x;
	m_render_location.y += vMin.y;

	if (Editor::the().has_file_loaded()) {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo());
		glViewport(0, 0, m_render_size.x, m_render_size.y);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.4f, 0.6f, 1.0f);

		editor.level_file()->render(m_camera, projection());

		const auto selected_mesh = m_currently_selected_mesh;
		if (selected_mesh != -1) {
			auto& mesh = Editor::the().level_file()->meshes()[selected_mesh];
			if (~mesh.mesh_data.flags & 1) {
				auto& na = mesh.mesh_data.not_flags_and_1;
				for (int i = 0; i < na.szme_data.size(); i++) {
					na.szme_data[i].render(m_camera, projection());
				}
			}
		}
	}


	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::BeginMenu("View")) {
			ImGui::MenuItem("Wireframe", nullptr, &config::the().renderer.wireframe);
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}

	ImGui::Image((ImTextureID)texture(), m_render_size, { 0, 1 }, { 1, 0 });
	char buf[64];
	sprintf(buf, "%.4f %.4f %.4f", camera().location().x, camera().location().y, camera().location().z);
	const auto text_size = ImGui::CalcTextSize(buf);
	ImGui::SetNextWindowSize({ text_size.x + 50.f, 60.0f });
	ImGui::SetNextWindowPos({ m_render_location.x, m_render_location.y });
	ImGui::SetNextWindowBgAlpha(0.5f);
	ImGui::Begin("asd", nullptr, wf::ImGuiWindowFlags_NoTitleBar | wf::ImGuiWindowFlags_NoMove | wf::ImGuiWindowFlags_NoCollapse | wf::ImGuiWindowFlags_NoDocking | wf::ImGuiWindowFlags_NoResize);
		ImGui::BeginChild("Coords", {}, false, wf::ImGuiWindowFlags_NoCollapse | wf::ImGuiWindowFlags_NoMove | wf::ImGuiWindowFlags_NoTitleBar | wf::ImGuiWindowFlags_NoDecoration | wf::ImGuiWindowFlags_NoResize | wf::ImGuiWindowFlags_NoScrollbar | wf::ImGuiWindowFlags_NoDocking);
			ImGui::SameLine((text_size.x + 50.f) / 2.0f - (text_size.x / 2.0f));
			ImGui::Text("%.4f %.4f %.4f", camera().location().x, camera().location().y, camera().location().z);
			ImGui::NewLine();
			ImGui::SameLine((text_size.x + 50.f) / 2.0f - (text_size.x / 2.0f));
			ImGui::Text("%.4f %.4f", camera().yaw(), camera().pitch());
		ImGui::EndChild();
	ImGui::End();
	ImGui::End();
}

void Renderer::handle_input(void* a_window, float delta_time)
{
	const auto window = (GLFWwindow*)a_window;
	const auto move_speed = config::the().renderer.move_speed;
	if (glfwGetKey(window, GLFW_KEY_W) > 0) {
		glm::vec3 rot = m_camera.forward_xy();
		m_camera.set_location(m_camera.location() + (rot * move_speed * delta_time));
		//dbgprint("%s\n", glm::to_string(g_camera.location()).c_str());
	}
	if (glfwGetKey(window, GLFW_KEY_A) > 0) {
		glm::vec3 left = m_camera.left();
		m_camera.set_location(m_camera.location() + (left * move_speed * delta_time));
	}
	if (glfwGetKey(window, GLFW_KEY_S) > 0) {
		glm::vec3 back = m_camera.back_xy();
		m_camera.set_location(m_camera.location() + (back * move_speed * delta_time));
	}
	if (glfwGetKey(window, GLFW_KEY_D) > 0) {
		glm::vec3 right = m_camera.right();
		m_camera.set_location(m_camera.location() + (right * move_speed * delta_time));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) > 0) {
		glm::vec3 up = m_camera.up();
		m_camera.set_location(m_camera.location() + (up * move_speed * delta_time));
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) > 0) {
		glm::vec3 up = m_camera.up();
		m_camera.set_location(m_camera.location() - (up * move_speed * delta_time));
	}
}

void Renderer::on_load()
{
	m_currently_selected_mesh = -1;
}

void Renderer::on_close()
{
	m_currently_selected_mesh = -1;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo());
	glViewport(0, 0, m_render_size.x, m_render_size.y);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

}

void Renderer::select(double mouse_x, double mouse_y, bool ctrl_modifier)
{
	if (!Editor::the().has_file_loaded())
		return;
	glm::vec3 ray = clickray(mouse_x, mouse_y, render_size().x, render_size().y, projection(), camera());
	std::vector<SlyMesh>& meshes = Editor::the().level_file()->meshes();
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
					const auto v1 = meshes[i].game_object().model() * glm::vec4(vertices[triangles[tri]].pos, 1.0f);
					const auto v2 = meshes[i].game_object().model() * glm::vec4(vertices[triangles[tri + 1]].pos, 1.0f);
					const auto v3 = meshes[i].game_object().model() * glm::vec4(vertices[triangles[tri + 2]].pos, 1.0f);
					if (ray_intersects_triangle(
						camera().location(),
						ray,
						v1,
						v2,
						v3,
						intersection_point
					)) {
						float len = glm::length(camera().location() - intersection_point);
						if (len < lowest_distance) {
							has_found = true;
							mesh = i;
							lowest_distance = len;
						}
					}
				}
			}
	}
	m_currently_selected_mesh = has_found ? mesh : -1;
}

void Renderer::calculate_projection_matrix()
{
	if (is_allocated())
	{
		m_projection = glm::perspective(glm::radians(config::the().renderer.fov), render_size().x / render_size().y, 0.2f, 650.0f);
		set_should_recalculate_projection(false);
	}
}