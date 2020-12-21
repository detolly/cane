#include "ModelBrowser.h"
#include <imgui/imgui.h>
#include <Utility/config.h>
#include <Editor.h>

void ModelBrowser::render()
{
	if (ImGui::Begin("Mesh Browser", &config::the().windows.mesh_browser)) {

	}
	ImGui::End();
}
