#include "ModelBrowser.h"
#include <imgui/imgui.h>
#include <Utility/config.h>
#include <Editor.h>

void ModelBrowser::render()
{
	if (ImGui::Begin("Model Browser", &config::the().windows.model_browser)) {

	}
	ImGui::End();
}
