#pragma once

#include <Object.h>
#include <FileReader.h>
#include <Camera.h>
#include <sly_structs.h>
#include <memory>

class SlyMesh : public SingleColoredWorldObject {

public:
	SlyMesh(mesh_data_t* data) : m_mesh_data(data) {
		parse();
		set_color(glm::vec3{ (float)(((uint32_t)(data) >> 12) % 255) / 255.0f, (float)(((uint32_t)(data) >> 8) % 255) / 255.0f, (float)(((uint32_t)(data) >> 4) % 255) / 255.0f });
		make_gl_buffers();
		game_object().set_constant_model(true);
	}

	void make_gl_buffers() {
		for (auto submesh : m_submeshes) {
			submesh->make_gl_buffers();
		}
	}

	void parse() {
		int current_offset = 0;
		if (!strstr(m_mesh_data->header.magic, "SZMS"))
			return;
		const auto mesh_count = m_mesh_data->mesh_hdr.mesh_count;
		const auto off = offsetof(mesh_data_t, mesh_hdr);
		mesh_header_t* mesh_header = (mesh_header_t*)((uint32_t)m_mesh_data + off);
		//printf("%08x ", (uint32_t)m_mesh_data);
		for (int i = 0; i < mesh_count; i++) {
			std::vector<glm::vec3> m_vertices_list;
			m_submeshes.push_back(std::make_shared<submesh>());
			uint32_t* offset_arr = (uint32_t*)(&m_mesh_data->mesh_hdr.mesh_offsets);
			vertex_data_t* vd = (vertex_data_t*)((uint32_t)mesh_header + offset_arr[i]);
			vertex_t* vertices = (vertex_t*)((uint32_t)mesh_header + vd->vertex_hdr.vertex_data_offset);
			for (int j = 0; j < vd->vertex_hdr.vertex_count; j++) {
				glm::vec3 v({ -vertices[j].pos_x, vertices[j].pos_z, vertices[j].pos_y });
				m_vertices_list.push_back(v/100.f);
			}

			index_header_t* index_header = (index_header_t*)((uint32_t)mesh_header + (uint32_t)vd->vertex_hdr.index_header_offset);
			uint16_t* triangle_data = (uint16_t*)((uint32_t)mesh_header + index_header->index_data_offset_0);
			for (int j = 0; j < index_header->triangle_count * 3; j++) {
				uint16_t triangle = triangle_data[j];
				m_submeshes[i]->vector().push_back(m_vertices_list[triangle]);
			}
		}
	}

	void render(Camera& cam, glm::mat4x4& proj) override
	{
		SingleColoredWorldObject::render(cam, proj);

		for (auto submesh : m_submeshes) {
			const auto id = submesh->vao();
			glBindVertexArray(id);
			glDrawArrays(GL_TRIANGLES, 0, submesh->vector().size()/3);
		}
	}

	class submesh {
	public:
		
		const unsigned vao() const { return m_vao; }
		const unsigned vertex_buffer_id() const { return m_vertex_buffer_id; }
		std::vector<glm::vec3>& vector() { return m_vertices; }

		void make_gl_buffers() {
			GLuint vao;
			GLuint vb;
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);
			glGenBuffers(1, &vb);

			m_vao = vao;
			m_vertex_buffer_id = vb;

			glBindBuffer(GL_ARRAY_BUFFER, vb);
			glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float) * 3, m_vertices.data(), GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glEnableVertexAttribArray(0);
		}

	private:
		GLuint m_vao{(unsigned int)-1};
		GLuint m_vertex_buffer_id{(unsigned int)-1};
		std::vector<glm::vec3> m_vertices;
	};

private:

	mesh_data_t* m_mesh_data;
	
	std::vector<std::shared_ptr<submesh>> m_submeshes;

};

class SlyMeshContainer : public RenderedWorldObject
{
public:
	SlyMeshContainer() {}

	SlyMeshContainer(const char* level_file) {
		FileReader reader(level_file, true);
		m_buffer = reader.read();
		m_buffer_len = reader.length();
		construct();
	}

	void construct()
	{
		const auto find = [](const char* b, const char* lf, int start, size_t len) {
			int already_found = 0;
			int look_len = std::strlen(lf);
			bool found{ false };
			for (int current_index = 0; (current_index + start) < len; current_index++) {
				const char* current = b + start + current_index - already_found;
				if (b[start + current_index] == lf[already_found])
					already_found++;
				else
					already_found = 0;
				if (already_found == look_len)
					return start + current_index - look_len + 1;
			}
			return -1;
		};

		size_t current_szme_index{0};
        while((current_szme_index = find(m_buffer, "SZMS", current_szme_index+4, m_buffer_len)) != -1) {
			SlyMesh mesh = SlyMesh((mesh_data_t*)(m_buffer + current_szme_index));
			mesh.parse();
			meshes.push_back(mesh);
			//break;
        }
	}

	void render(Camera& cam, glm::mat4& matrix) override {
		for (int i = 0; i < meshes.size(); i++) {
			meshes[i].render(cam, matrix);
		}
	}

private:
	const char* m_buffer{nullptr};
	size_t m_buffer_len{0};

	std::vector<SlyMesh> meshes;

};