//
// Created by Thomas on 1/21/2021.
//

#include "RendererOptions.h"
#include <Utility/config.h>
#include <Structs/SlyLevelFile.h>
#include <Editor.h>
#include <imgui.h>

void RendererOptions::render() {
    if (ImGui::Begin("Renderer Options", &config::the().windows.renderer_options))
    {
        if (Editor::the().has_file_loaded())
        {
            int i = 0;
            char buf[50];
            for(auto& unknown_vector_array : Editor::the().level_file()->unknown_vector_arrays()) {
                sprintf_s(buf, 50, "Unknown Array %d", i++);
                if (ImGui::CollapsingHeader(buf)) {
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
