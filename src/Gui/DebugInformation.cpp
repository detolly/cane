#include "DebugInformation.h"
#include <Gui/Renderer.h>
#include <Structs/SlyLevelFile.h>
#include <Editor.h>
#include <imgui/imgui.h>

void DebugInformation::render()
{
	int currently_selected_mesh = Renderer::the().currently_selected_mesh();
	ImGui::Begin("Debug Information", &config::the().windows.debug_information);
	ImGui::TextWrapped("Selected Mesh: ");
	if (Editor::the().has_file_loaded() && currently_selected_mesh > -1) {
		auto& writable_mesh = Editor::the().level_file()->meshes()[currently_selected_mesh];
		ImGui::DragFloat3("World Coords", (float*)&writable_mesh.game_object().raw_location(), 0.1f, 0.0f, 0.0f, "%.3f", 1.0f);
		ImGui::DragFloat3("Model Rotation", (float*)&writable_mesh.game_object().raw_rotation(), 0.1f, 0.0f, 360.0f, "%.3f", 1.0f);
		ImGui::DragFloat3("Model Scale", (float*)&writable_mesh.game_object().raw_scale(), 0.01f, 0.0f, 0.0f, "%.3f", 1.0f);
		if (Editor::the().io()->WantCaptureMouse) {
            writable_mesh.game_object().set_should_recalculate_model_matrix(true);
            writable_mesh.game_object().calculate_model_matrix_if_needed();
		}
		const auto& mesh = Editor::the().level_file()->meshes()[currently_selected_mesh];
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
			for (size_t i = 0; i < na.szme_data.size(); i++) {
			    const auto& data = na.szme_data[i];
				char buf[32];
				sprintf_s(buf, 32, "SZME Data #%d", i);
				if (ImGui::CollapsingHeader(buf)) {
					ImGui::Indent();
					ImGui::TextWrapped("vertex_count %03d\tnormal_count %03d\tvertex_color_count %03d\ttexcoords_count %03d\tindex_count %03d", data.vertex_count, data.normal_count, data.vertex_color_count, data.texcoords_count, data.index_count);
					if (ImGui::CollapsingHeader("Vertices")) {
						for (size_t j = 0; j < data.vertices.size(); j++) {
							ImGui::TextWrapped("%f %f %f", data.vertices[j].x, data.vertices[j].y, data.vertices[j].z);
						}
					}
					if (ImGui::CollapsingHeader("Normals")) {
						for (size_t j = 0; j < data.normals.size(); j++) {
							ImGui::TextWrapped("%f %f %f", data.normals[j].x, data.normals[j].y, data.normals[j].z);
						}
					}
					if (ImGui::CollapsingHeader("Texcoords")) {
						for (size_t j = 0; j < data.texcoords.size(); j++) {
							ImGui::TextWrapped("%f %f", data.texcoords[j].u, data.texcoords[j].v);
						}
					}
					if (ImGui::CollapsingHeader("Indices")) {
						for (size_t j = 0; j < data.indices.size(); j++) {
							ImGui::TextWrapped("Position: %d Normal: %d Texcoords: %d Unk: %d", data.indices[j].vertex_index, data.indices[j].normal_index, data.indices[j].texcoords_index, data.indices[j].unk);
						}
					}
					if (ImGui::CollapsingHeader("Other Information")) {
						ImGui::TextWrapped("unk_u8_1 %03x (%03d)", data.unk_u8_1, data.unk_u8_1);
						ImGui::TextWrapped("unk_u8_2 %03x (%03d)", data.unk_u8_2, data.unk_u8_2);
						ImGui::TextWrapped("unk_vec3 %.4f %.4f %.4f", data.unk_vec.x, data.unk_vec.y, data.unk_vec.z);
						ImGui::TextWrapped("Texture Id: %d", data.texture_id);
						const auto* const_level = Editor::the().level_file();
						if (data.texture_id < const_level->texture_table().texture.size())
							ImGui::Image(
								(ImTextureID)const_level->texture_table().texture[data.texture_id].gl_texture,
								{ 256.0f, 256.0f },
								{ 0.0f, 1.0f }, { 1.0f, 0.0f }
							);
						else
						    ImGui::TextWrapped("TextureID is invalid (%d outside the bounds of array)", data.texture_id);
					}
					ImGui::Unindent();
				}
			}
		}
	}
	ImGui::End();
}

void DebugInformation::on_load()
{
}

void DebugInformation::on_close()
{
}
