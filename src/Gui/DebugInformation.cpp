#include "DebugInformation.h"
#include <Gui/RendererWindow.h>
#include <Structs/SlyLevelFile.h>
#include <Editor.h>
#include <imgui/imgui.h>

void DebugInformation::render()
{
	const auto currently_selected_mesh = Editor::the().renderer()->currently_selected_mesh();
	ImGui::Begin("Debug Information", &config::the().windows.debug_information);
	ImGui::TextWrapped("Selected Mesh: ");
	if (Editor::the().has_file_loaded() && currently_selected_mesh > -1) {
		auto& mesh = Editor::the().level_file()->meshes()[currently_selected_mesh];
		ImGui::DragFloat3("World Coords", (float*)&mesh.game_object().raw_location(), 0.1f, 0.0f, 0.0f, "%.3f", 1.0f);
		ImGui::DragFloat3("Model Rotation", (float*)&mesh.game_object().raw_rotation(), 0.1f, 0.0f, 360.0f, "%.3f", 1.0f);
		ImGui::DragFloat3("Model Scale", (float*)&mesh.game_object().raw_scale(), 0.01f, 0.0f, 0.0f, "%.3f", 1.0f);
		if (~mesh.mesh_data.flags & 1) {
			ImGui::TextWrapped("Mesh Header:");
			auto& na = mesh.mesh_data.not_flags_and_1;
			ImGui::TextWrapped("Unknown 0x00 0x%08x", na.mesh_hdr.unknown_0x00);
			ImGui::TextWrapped("Unknown 0x04 0x%04x", na.mesh_hdr.unknown_0x04);
			ImGui::TextWrapped("Mesh Count: %d\nUK1: %d | UK2: %d", na.mesh_hdr.mesh_count, na.mesh_hdr.unknown_0x00, na.mesh_hdr.unknown_0x04);

			auto& header = mesh.mesh_data.not_flags_and_1.szme_hdr;
			ImGui::TextWrapped("SZME Header: ");
			ImGui::TextWrapped("Flags: 0x%08x", mesh.mesh_data.flags);
			ImGui::TextWrapped("SZME Mesh Count: %d", mesh.mesh_data.not_flags_and_1.szme_hdr.m.mesh_count);
			ImGui::TextWrapped("Relevant Flag Information:");
			ImGui::TextWrapped(" Flag & 0x2  \t Uint32_t 0x%08x", header.m.unk_0x04);
			ImGui::TextWrapped(" Flag & 0x200\t Float %.4f", header.m.unk_float);
			ImGui::TextWrapped(" Flag & 0x4  \t Float %.4f", header.m.unk_float2);
			ImGui::TextWrapped(" Flag & 0x8  \t Float %.4f", header.m.unk_float3);
			ImGui::TextWrapped(" Flag & 0x10 \t Float %.4f", header.m.unk_float4);
			ImGui::TextWrapped(" Flag & 0x20 \t Float %.4f", header.m.unk_float5);
			ImGui::TextWrapped("~Flag & 0x100\t Float %.4f", header.m.unk_0x14);
			ImGui::TextWrapped("~Flag & 0x100\t Vec3 %.4f %.4f %.4f (O position)", header.m.position.x, header.m.position.y, header.m.position.z);
			ImGui::TextWrapped("~Flag & 0x100\t unk_0x16_ignore: 0x%08x (%9d)", header.m.unk_0x16_ignore, header.m.unk_0x16_ignore);
			ImGui::TextWrapped("~Flag & 0x100\t unk_byte_1A: %03x (%03d)", header.m.unk_0x1A, header.m.unk_0x1A);
			ImGui::TextWrapped("~Flag & 0x100\t unk_byte_1B: %03x (%03d)", header.m.unk_0x1B, header.m.unk_0x1B);
			ImGui::TextWrapped("~Flag & 0x100\t unk_byte_1C: %03x (%03d)", header.m.unk_0x1C, header.m.unk_0x1C);
			uint16_t* ptr = (uint16_t*)&header.m.unk_0x1A;
			uint16_t* ptr2 = (uint16_t*)&header.m.unk_0x1B;
			ImGui::TextWrapped("~Flag & 0x100\t unk_byte_1A1B: %04x (%05d)", *ptr, *ptr);
			ImGui::TextWrapped("~Flag & 0x100\t unk_byte_1B1C: %04x (%05d)", *ptr2, *ptr2);

			ImGui::TextWrapped("SZME Data: ");
			for (int i = 0; i < na.szme_data.size(); i++) {
				char buf[32];
				sprintf(buf, "SZME Data #%d", i);
				if (ImGui::CollapsingHeader(buf)) {
					ImGui::Indent();
					ImGui::TextWrapped("position_count %03d\trotation_count %03d\tuc3 %03d\ttexcoords_count %03d\tlighing_count %03d", na.szme_data[i].vertex_count, na.szme_data[i].normal_count, na.szme_data[i].unk_count3, na.szme_data[i].texcoords_count, na.szme_data[i].index_count);
					if (ImGui::CollapsingHeader("Vertices")) {
						for (int j = 0; j < na.szme_data[i].vertices.size(); j++) {
							ImGui::TextWrapped("%f %f %f", na.szme_data[i].vertices[j].x, na.szme_data[i].vertices[j].y, na.szme_data[i].vertices[j].z);
						}
					}
					if (ImGui::CollapsingHeader("Normals")) {
						for (int j = 0; j < na.szme_data[i].normals.size(); j++) {
							ImGui::TextWrapped("%f %f %f", na.szme_data[i].normals[j].x, na.szme_data[i].normals[j].y, na.szme_data[i].normals[j].z);
						}
					}
					if (ImGui::CollapsingHeader("Texcoords")) {
						for (int j = 0; j < na.szme_data[i].texcoords.size(); j++) {
							ImGui::TextWrapped("%f %f", na.szme_data[i].texcoords[j].u, na.szme_data[i].texcoords[j].v);
						}
					}
					if (ImGui::CollapsingHeader("Indices")) {
						for (int j = 0; j < na.szme_data[i].indices.size(); j++) {
							ImGui::TextWrapped("Position: %d Normal: %d Texcoords: %d Unk: %d", na.szme_data[i].indices[j].vertex_index, na.szme_data[i].indices[j].normal_index, na.szme_data[i].indices[j].texcoords_index, na.szme_data[i].indices[j].unk);
						}
					}
					if (ImGui::CollapsingHeader("Other Information")) {
						ImGui::TextWrapped("unk_u8_1 %03x (%03d)", na.szme_data[i].unk_u8_1, na.szme_data[i].unk_u8_1);
						ImGui::TextWrapped("unk_u8_2 %03x (%03d)", na.szme_data[i].unk_u8_2, na.szme_data[i].unk_u8_2);
						ImGui::TextWrapped("unk_vec3 %.4f %.4f %.4f", na.szme_data[i].unk_vec.x, na.szme_data[i].unk_vec.y, na.szme_data[i].unk_vec.z);
						ImGui::TextWrapped("Texture Id: %d", na.szme_data[i].texture_id);
						if (na.szme_data[i].texture_id < Editor::the().level_file()->texture_table().texture.size())
							ImGui::Image(
								(ImTextureID)Editor::the().level_file()->texture_table().texture[na.szme_data[i].texture_id].gl_texture,
								{ 256.0f, 256.0f },
								{ 0.0f, 1.0f }, { 1.0f, 0.0f }
							);
					}
					ImGui::Unindent();
				}
			}
		}
		mesh.game_object().set_should_recalculate_model_matrix(true);
	}
	ImGui::End();
}

void DebugInformation::on_load()
{
}

void DebugInformation::on_close()
{
}
