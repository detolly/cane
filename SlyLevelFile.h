#pragma once

#include <Object.h>
#include <FileReader.h>
#include <Camera.h>
#include <sly_level_structs.h>
#include <sly_texture_structs.h>
#include <memory>
#include <ez_stream.h>

class SlyMesh : public SingleColoredWorldObject {

public:
	SlyMesh(ez_stream& stream) {
		parse(stream);
		set_color(glm::vec3{
			(float)(stream.tell() / 1 % 255) / 255.0f,
			(float)(stream.tell() / 3 % 255) / 255.0f,
			(float)(stream.tell() / 7 % 255) / 255.0f }
		);
		make_gl_buffers();
		//game_object().set_constant_model(true);
	}

	void parse(ez_stream& stream) {
		mesh_data = mesh_data_t(stream);
	}

	mesh_data_t mesh_data;

	void make_gl_buffers() {
		if (~mesh_data.flags & 1)
			for (int i = 0; i < mesh_data.flags_and_1.vertex_data.size(); i++) {

			}
	}

	void render(Camera& cam, glm::mat4x4& proj) override
	{
		SingleColoredWorldObject::render(cam, proj);

		//for (auto submesh : m_submeshes) {
		//	const auto id = submesh->vao();
		//	glBindVertexArray(id);
		//	glDrawArrays(GL_TRIANGLES, 0, submesh->vector().size()/3);
		//}
	}
};

class BitmapCreator {

public:

	BitmapCreator()
	{}

	struct RGBA {
		unsigned char r, g, b, a;
	};

private:
	RGBA* m_bitmap;
	size_t m_bitmap_len;

};

class SlyLevelFile : public RenderedWorldObject
{
public:
	SlyLevelFile() {}

	SlyLevelFile(const char* level_file) {
		FileReader reader(level_file, true);
		m_buffer = reader.read();
		m_buffer_len = reader.length();
		construct();
	}

	~SlyLevelFile() {
		delete[] m_buffer;
	}

	void construct()
	{
		ez_stream stream(m_buffer, m_buffer_len);
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
			stream.seek(current_szme_index-2);
			m_meshes.push_back(std::move(SlyMesh(stream)));
        }

		size_t finding_texture_table = 0;
		int s = 0;
		while ((s = find(m_buffer, "FK$Dcrmtaunt07", s + 4, m_buffer_len)) != -1)
			finding_texture_table = s;
		finding_texture_table += 32;
		stream.seek(finding_texture_table);
		m_clut_meta_table = clut_meta_table_t(stream);
		m_image_meta_table = image_meta_table_t(stream);
		m_texture_table = texture_table_t(stream);
		for (int i = 0; i < 250; i++) {
			unk_table[i] = stream.read<UNK_TABLE_t>();
		}
	}

	void render(Camera& cam, glm::mat4& matrix) override {
		for (int i = 0; i < m_meshes.size(); i++) {
			m_meshes[i].render(cam, matrix);
		}
	}

private:
	const char* m_buffer{nullptr};
	size_t m_buffer_len{0};

	std::vector<SlyMesh> m_meshes;

	clut_meta_table_t m_clut_meta_table;
	image_meta_table_t m_image_meta_table;
	texture_table_t m_texture_table;
	UNK_TABLE_t unk_table[250];
};