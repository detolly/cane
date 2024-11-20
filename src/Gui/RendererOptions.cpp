#include "RendererOptions.h"

#include <format>

#include <imgui.h>

#include <Editor.h>
#include <Structs/SlyWorld.h>
#include <Utility/config.h>

void RendererOptions::render() {
    if (ImGui::Begin("Renderer Options", &config::the().windows.renderer_options))
    {
        if (Editor::the().has_file_loaded())
        {
            int i = 0;
            for(auto& unknown_vector_array : Editor::the().level_file()->unknown_vector_arrays()) {
                const auto buf = std::format("Unknown Array {}", i++);
                if (ImGui::CollapsingHeader(buf.c_str())) {
                    ImGui::Indent();
                    ImGui::Checkbox("Should Render", &unknown_vector_array.m_should_draw);
                    const char* const items[] = { "Triangles", "Lines", "Points" };
                    ImGui::ListBox("Render Function", (int*)&unknown_vector_array.m_draw_func, items, IM_ARRAYSIZE(items));
                    ImGui::Unindent();
                }
            }
        }
        ImGui::End();
    }
}

void RendererOptions::on_load()
{

}

void RendererOptions::on_close()
{

}

