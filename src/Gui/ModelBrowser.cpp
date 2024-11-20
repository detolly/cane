#include "ModelBrowser.h"

#include <GL/glew.h>
#include <imgui/imgui.h>

#include <Editor.h>
#include <Gui/ModelViewer.h>
#include <Structs/SlyWorld.h>
#include <Utility/config.h>


void ModelBrowser::render()
{
    ImGui::Begin("Model Browser", &config::the().windows.model_browser);
    ImGui::BeginGroup();
    for (Thumbnail& t : m_thumbnails) {
        if (ImGui::GetContentRegionAvail().x < 128)
            ImGui::NewLine();
        if (ImGui::ImageButton(std::to_string(t.index()).c_str(),static_cast<ImTextureID>(t.texture()), { 128, 128 }, { 0, 1 }, { 1, 0 })) {
            ModelViewer::the().set_selected(t.index());
        }
        ImGui::SameLine();
    }
    ImGui::EndGroup();
    ImGui::End();
}

void ModelBrowser::make_thumbnails() {
    m_thumbnails.clear();
    const auto& level = Editor::the().level_file();
    for (size_t i = 0; i < level->meshes().size(); i++) {
        const auto& mesh = level->meshes()[i];
        Thumbnail b(i);
        b.render(*mesh);
        m_thumbnails.emplace_back(std::move(b));
    }
}

void Thumbnail::render(const SlyMesh& mesh) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo());

    glViewport(0, 0, 128, 128);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    static Camera cam{};
    cam.set_location(mesh.game_object().location() + 10.0f);
    cam.set_pitch(-45.0f);
    cam.set_yaw(-135.0f);
    cam.calculate_view_matrix_if_needed();
    const static auto proj = glm::perspective(90.0f, 1.0f, 0.1f, 50.0f);

    mesh.render(cam, proj);
}

