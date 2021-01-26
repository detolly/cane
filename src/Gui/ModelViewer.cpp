#include "ModelViewer.h"
#include <imgui/imgui.h>
#include <glad/glad.h>
#include <Utility/Config.h>
#include <Renderer/Camera.h>
#include <Editor.h>
#include <Structs/SlyLevelFile.h>
#include <cmath>

void ModelViewer::init() {
    resize_buffer(512, 512);
}

void ModelViewer::render()
{
	ImGui::Begin("Model Viewer", &config::the().windows.model_viewer);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo());
	glViewport(0, 0, 512, 512);

	const auto rgba = config::the().model_viewer.rgba;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(rgba.r, rgba.g, rgba.b, rgba.a);

	if (m_selected != -1) {
		auto& mesh = Editor::the().level_file()->meshes()[m_selected];

		static glm::mat4 proj = glm::perspective(90.0f, 1.0f, 0.1f, 50.0f);
		static Camera cam{};
		cam.set_location(mesh.game_object().location() + 10.0f);
		cam.set_pitch(-45.0f);
		cam.set_yaw(-135.0f);

		mesh.render(cam, proj);
		const auto size = ImGui::GetContentRegionAvail();
		const auto val = std::fmin(std::fmin(size.x, size.y), 512.0f);
		ImGui::SameLine(size.x / 2.0f - val/2.0f);
		ImGui::Image((ImTextureID)texture(), { val, val }, { 0, 1 }, { 1, 0 });
	}
	ImGui::End();
}

void ModelViewer::set_selected(int value)
{
	m_selected = value;
}
