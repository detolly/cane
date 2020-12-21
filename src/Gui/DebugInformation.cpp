#include "DebugInformation.h"
#include <Editor.h>

void DebugInformation::render()
{
	const auto currently_selected_mesh = Editor::the().renderer().currently_selected_mesh();
	if (ImGui::Begin("Debug Information", &config::the().windows.debug_information)) {
		ImGui::Text("Selected Mesh: ");
		if (currently_selected_mesh > -1) {
			auto& mesh = Editor::the().level_file()->meshes()[currently_selected_mesh];
			ImGui::DragFloat3("XYZ", (float*)&mesh.game_object().raw_location(), 0.1f, 0.0f, 0.0f, "%.3f", 1.0f);
			if (~mesh.mesh_data.flags & 1) {
				ImGui::Text("Mesh Header:");
				auto& na = mesh.mesh_data.not_flags_and_1;
				ImGui::Text("Unknown 0x00 0x%08x", na.mesh_hdr.unknown_0x00);
				ImGui::Text("Unknown 0x04 0x%04x", na.mesh_hdr.unknown_0x04);
				ImGui::Text("Mesh Count: %d\nUK1: %d | UK2: %d", na.mesh_hdr.mesh_count, na.mesh_hdr.unknown_0x00, na.mesh_hdr.unknown_0x04);

				auto& header = mesh.mesh_data.not_flags_and_1.szme_hdr;
				ImGui::Text("SZME Header: ");
				ImGui::Text("Flags: 0x%08x", mesh.mesh_data.flags);
				ImGui::Text("Relevant Flag Information:");
				ImGui::Text(" Flag & 0x2  \t Uint32_t 0x%08x", header.m.unk_0x04);
				ImGui::Text(" Flag & 0x200\t Float %.4f", header.m.unk_float);
				ImGui::Text(" Flag & 0x4  \t Float %.4f", header.m.unk_float2);
				ImGui::Text(" Flag & 0x8  \t Float %.4f", header.m.unk_float3);
				ImGui::Text(" Flag & 0x10 \t Float %.4f", header.m.unk_float4);
				ImGui::Text(" Flag & 0x20 \t Float %.4f", header.m.unk_float5);
				ImGui::Text("~Flag & 0x100\t Float %.4f", header.m.unk_0x14);
				ImGui::Text("~Flag & 0x100\t Vec3 %.4f %.4f %.4f (O position)", header.m.position.x, header.m.position.y, header.m.position.z);
				ImGui::Text("~Flag & 0x100\t unk_0x16_ignore: 0x%08x (%9d)", header.m.unk_0x16_ignore, header.m.unk_0x16_ignore);
				ImGui::Text("~Flag & 0x100\t unk_byte_1A: %03x (%03d)", header.m.unk_0x1A, header.m.unk_0x1A);
				ImGui::Text("~Flag & 0x100\t unk_byte_1B: %03x (%03d)", header.m.unk_0x1B, header.m.unk_0x1B);
				ImGui::Text("~Flag & 0x100\t unk_byte_1C: %03x (%03d)", header.m.unk_0x1C, header.m.unk_0x1C);
				uint16_t* ptr = (uint16_t*)&header.m.unk_0x1A;
				uint16_t* ptr2 = (uint16_t*)&header.m.unk_0x1B;
				ImGui::Text("~Flag & 0x100\t unk_byte_1A1B: %04x (%05d)", *ptr, *ptr);
				ImGui::Text("~Flag & 0x100\t unk_byte_1B1C: %04x (%05d)", *ptr2, *ptr2);

				ImGui::Text("SZME Data: ");
				for (int i = 0; i < na.szme_data.size(); i++) {
					char buf[32];
					sprintf(buf, "SZME Data #%d", i);
					if (ImGui::CollapsingHeader(buf)) {
						ImGui::Text("unk_u8_1 %03x (%03d)", na.szme_data[i].unk_u8_1, na.szme_data[i].unk_u8_1);
						ImGui::Text("unk_u8_2 %03x (%03d)", na.szme_data[i].unk_u8_2, na.szme_data[i].unk_u8_2);
						ImGui::Text("UnkVec %.4f %.4f %.4f", na.szme_data[i].unk_vec.x, na.szme_data[i].unk_vec.y, na.szme_data[i].unk_vec.z);
						ImGui::Text("position_count %03d\trotation_count %03d\tuc3 %03d\ttexcoords_count %03d\tlighing_count %03d", na.szme_data[i].position_count, na.szme_data[i].rotation_count, na.szme_data[i].unk_count3, na.szme_data[i].texcoords_count, na.szme_data[i].lighing_count);
						ImGui::Text("Positions information: ");
						for (int j = 0; j < na.szme_data[i].positions.size(); j++) {
							ImGui::Text("%f %f %f", na.szme_data[i].positions[j].x, na.szme_data[i].positions[j].y, na.szme_data[i].positions[j].z);
						}
					}
				}
			}
			mesh.game_object().set_should_recalculate_model_matrix(true);
		}
	}
	ImGui::End();
}
