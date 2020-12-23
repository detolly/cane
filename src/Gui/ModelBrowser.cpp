#include "ModelBrowser.h"
#include <imgui/imgui.h>
#include <Utility/config.h>
#include <Editor.h>
#include <Structs/SlyLevelFile.h>
#include <glad/glad.h>

void ModelBrowser::render()
{
	ImGui::Begin("Model Browser", &config::the().windows.model_browser);
	ImGui::BeginGroup();
	for (Thumbnail& t : m_thumbnails) {
		if (ImGui::GetContentRegionAvail().x < 128)
			ImGui::NewLine();
		if (ImGui::ImageButton((ImTextureID)t.texture(), { 128, 128 }, { 0, 1 }, { 1, 0 })) {
			Editor::the().model_viewer()->set_selected(t.index());
		}
		ImGui::SameLine();
	}
	ImGui::EndGroup();
	ImGui::End();
}

void ModelBrowser::make_thumbnails() {
	m_thumbnails.clear();
	auto* level = Editor::the().level_file();
	for (int i = 0; i < level->meshes().size(); i++) {
		auto& mesh = level->meshes()[i];
		Thumbnail b(i);
		b.render(mesh);
		m_thumbnails.push_back(std::move(b));
	}
}

void Thumbnail::render(SlyMesh& mesh)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo());

	glViewport(0, 0, 128, 128);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	static Camera cam{};
	cam.set_location(mesh.game_object().location() + 10.0f);
	cam.set_pitch(-45.0f);
	cam.set_yaw(-135.0f);
	static glm::mat4 proj = glm::perspective(90.0f, 1.0f, 0.1f, 50.0f);

	mesh.render(cam, proj);
}
