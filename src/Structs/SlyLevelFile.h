#pragma once

#include <main.h>

#include <Renderer/Object.h>
#include <Renderer/Camera.h>
#include <Structs/sly_level_structs.h>
#include <Structs/sly_texture_structs.h>
#include <memory>
#include <Utility/ez_stream.h>
#include <Utility/Sigscan.h>
#include <Utility/FileReader.h>

#include <glad/glad.h>
#include <gl/GL.h>


class SlyMesh : public SingleColoredSlyWorldObject {

public:
	texture_table_t& m_texture_table;

	SlyMesh() = delete;
	~SlyMesh() = default;
	SlyMesh(const SlyMesh & o) = default;
	SlyMesh(SlyMesh&& o) = default;
	SlyMesh& operator=(SlyMesh && o) = default;
	SlyMesh(ez_stream& stream, texture_table_t& texture_table) : m_texture_table(texture_table) {
		parse(stream);
		//set_color(glm::vec3{
		//	(float)(stream.tell() / 1 % 255) / 255.0f,
		//	(float)(stream.tell() / 3 % 255) / 255.0f,
		//	(float)(stream.tell() / 7 % 255) / 255.0f }
		//);
		set_color({0.5f, 0.5f, 0.5f});
		//if (mesh_data.not_flags_and_1.szme_data.size() > 0)
			//dbgprint("Mesh has texture id of: %d\n", mesh_data.not_flags_and_1.szme_data[0].texture_id);
		make_gl_buffers();
		//game_object().set_scale({ 1.f/100.f, 1.f/100.f, 1.f/100.f });
	}


	void parse(ez_stream& stream) {
		mesh_data = std::move(mesh_data_t(stream));
		auto p = mesh_data.not_flags_and_1.szme_hdr.m.position;
		game_object().set_location(glm::vec3(p.x, p.y, p.z));
	}

	mesh_data_t mesh_data;

	void make_gl_buffers() {
		if (~mesh_data.flags & 1)
		{
			mesh_data.not_flags_and_1.render_properties_vector.resize(mesh_data.not_flags_and_1.mesh_hdr.mesh_count);
			for (int i = 0; i < mesh_data.not_flags_and_1.mesh_hdr.mesh_count; i++) {
				GLuint VAO, VBO, EBO;
				glGenVertexArrays(1, &VAO);
				glGenBuffers(1, &VBO);
				glGenBuffers(1, &EBO);
				glBindVertexArray(VAO);
				mesh_data.not_flags_and_1.render_properties_vector[i] = {VAO, VBO, EBO};
				
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, mesh_data.not_flags_and_1.vertex_data[i].vertices.size() * sizeof(vertex_t), mesh_data.not_flags_and_1.vertex_data[i].vertices.data(), GL_STATIC_DRAW);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_data.not_flags_and_1.vertex_data[i].index_hdr.triangle_data.size() * sizeof(uint16_t),
					mesh_data.not_flags_and_1.vertex_data[i].index_hdr.triangle_data.data(), GL_STATIC_DRAW);
				
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)0);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)12);
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)24);
				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(vertex_t), (void*)32);

				glBindVertexArray(0);
			}
		}
	}

	void free_gl_buffers()
	{

	}

	void render(Camera& cam, glm::mat4x4& proj) override
	{
		if (mesh_data.flags & 1)
			return;

		SingleColoredSlyWorldObject::render(cam, proj);

		for (int i = 0; i < mesh_data.not_flags_and_1.mesh_hdr.mesh_count; i++) {
			int t_id = 0;
			if (mesh_data.not_flags_and_1.szme_data.size() >= i + 1)
				if (mesh_data.not_flags_and_1.szme_data[i].texture_id < m_texture_table.texture.size())
					if (m_texture_table.texture[mesh_data.not_flags_and_1.szme_data[i].texture_id].is_initialized())
						glBindTexture(GL_TEXTURE_2D, m_texture_table.texture[mesh_data.not_flags_and_1.szme_data[i].texture_id].gl_texture);
					else glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(mesh_data.not_flags_and_1.render_properties_vector[i].vao);
			glDrawElements(GL_TRIANGLES, mesh_data.not_flags_and_1.vertex_data[i].index_hdr.triangle_data.size(), GL_UNSIGNED_SHORT, 0);
			//glBindVertexArray(0);
		}
	}
};

class SlyLevelFile : public RenderedWorldObject
{
private:
	SlyLevelFile() {}
public:

	SlyLevelFile(const char* level_file) {
		FileReader reader(level_file);
		const auto* bf = reader.read();
		const auto sz = reader.length();
		ez_stream stream(bf, sz);
		parse_textures(stream);
		parse_meshes(stream);
	}

	~SlyLevelFile() {
		for (auto& mesh : meshes()) {
			for (auto& rp : mesh.mesh_data.not_flags_and_1.render_properties_vector) {
				glDeleteBuffers(1, &rp.ebo);
				glDeleteBuffers(1, &rp.vbo);
				glDeleteVertexArrays(1, &rp.vao);
			}
			for (auto& md : mesh.mesh_data.not_flags_and_1.szme_data) {
				md.free_gl_buffers();
			}
		}
		for (auto& tex_record : m_texture_table.texture) {
			glDeleteTextures(1, &tex_record.gl_texture);
		}
	}

	static int find(const char* b, const char* lf, int start, size_t len) {
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
	}

	void construct()
	{
	}

	//TODO REMOVE DEBUG
	void find_near_float(ez_stream& stream, float x, float y, float z, float allowed_difference) {
		for (int i = 0; i < stream.size(); i++) {
			float* ptr1 = (float*)(stream.buffer() + i);
			float* ptr2 = (float*)(stream.buffer() + i + 4);
			float* ptr3 = (float*)(stream.buffer() + i + 8);
			if (std::fabs(*ptr1 - x) < allowed_difference) {
				//dbgprint("Found X. (0x%08x) %f %f %f\n", i, *ptr1, *ptr2, *ptr3);
				if (std::fabs(*ptr2 - y) < allowed_difference) {
					if (std::fabs(*ptr3 - z) < allowed_difference) {
						dbgprint("FOUND @ 0x%08x\n", i);
					}
				}
			}
		}
	}

	void parse_textures(ez_stream& stream)
	{
		size_t finding_texture_table = 0;
		int s = 0;
		while ((s = find(stream.buffer(), "FK$Dcrmtaunt07", s + 4, stream.size())) != -1)
			finding_texture_table = s;
		finding_texture_table += 32;
		stream.seek(finding_texture_table);
		m_clut_meta_table = clut_meta_table_t(stream);
		m_image_meta_table = image_meta_table_t(stream);
		m_texture_table = texture_table_t(stream);
		for (int i = 0; i < 250; i++) {
			unk_table[i] = stream.read<UNK_TABLE_t>();
		}

		for (int i = 0; i < 0x100; i += 0x20) {
			for (int j = i; j < i + 8; j++) {
				csm1ClutIndices[j] = j;
				csm1ClutIndices[j + 8] = j + 0x10;
				csm1ClutIndices[j + 0x10] = j + 0x8;
				csm1ClutIndices[j + 0x18] = j + 0x18;
			}
		}

		int offset = detolly::sigscan(
			stream.buffer(),
			0,
			stream.size(),
			" aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80", "aaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaax",
			0
		);
		//dbgprint("%08x\n", offset);
		m_TEX_PALETTE_BASE = offset;

		int tex_entry_idx = 0;

		for (auto& texture_record : m_texture_table.texture) {

			const bool is1Img1Pal = (texture_record.clut_index.size() == texture_record.image_index.size());
			const bool is1ImgManyPal = (texture_record.image_index.size() == 1) && (texture_record.clut_index.size() > 1);
			const bool isManyImgManyPal = !is1Img1Pal && (texture_record.image_index.size() > 1) && (texture_record.clut_index.size() > 1);

			if (is1Img1Pal) {
				for (int i = 0; i < texture_record.clut_index.size(); i++) {
					if (i == ((texture_record.clut_index.size() / 2)))
						make_texture(stream.buffer(), texture_record, texture_record.clut_index[i], texture_record.image_index[i]);
				}
			}
			else if (is1ImgManyPal) {
				for (int i = 0; i < texture_record.clut_index.size(); i++) {
					if (i == ((texture_record.clut_index.size() / 2)))
						make_texture(stream.buffer(), texture_record, texture_record.clut_index[i], texture_record.image_index[0]);
				}
			}
			else if (isManyImgManyPal) {
				//if (!Number.isInteger(texEntry.clutIndices.length / texEntry.imageIndices.length)) {
				//	console.log(`WARN: nonint m2m ${ texEntryIdx } ${ texEntry.clutIndices.length } ${ texEntry.imageIndices.length }`);
				//}
				const auto divPalImg = texture_record.clut_index.size() / texture_record.image_index.size();
				for (int i = 0; i < texture_record.clut_index.size(); ++i) {
					int imgIndex = i / divPalImg;
					if (i == ((texture_record.clut_index.size() / 2)))
						make_texture(stream.buffer(), texture_record, texture_record.clut_index[i], texture_record.image_index[imgIndex]);
				}
			}
			else {
				//console.log(`WARN: other ${ texEntryIdx } ${ texEntry.clutIndices.length } ${ texEntry.imageIndices.length }`);
			}
			tex_entry_idx++;
		}
	}

	void parse_meshes(ez_stream& stream)
	{
		int total = 0;
		size_t current_szme_index{ 0 };
		while ((current_szme_index = find(stream.buffer(), "SZMS", current_szme_index + 4, stream.size())) != -1) {
			stream.seek(current_szme_index - 2);
			total++;
			//dbgprint("Found another object.. Total: %d\r\n", total);
			try {
				m_meshes.push_back(std::move(SlyMesh(stream, m_texture_table)));
			}
			catch (std::exception& e) {
				//dbgprint(e.what());
				break;
			}
		}
	}

	void make_texture(const char* buffer, texture_record_t& tex, int clutIndex, int imageIndex) {
		if (clutIndex >= m_clut_meta_table.record.size()) {
			//dbgprint("warn: clutIndex(%d) out of bounds, skipping\n", clutIndex);
			return;
		}
		if (imageIndex >= m_image_meta_table.header.numRecords) {
			//dbgprint("warn: imageIndex(%d) out of bounds, skipping\n", imageIndex);
			return;
		}
		 
		const clut_record_t& clutMeta = m_clut_meta_table.record[clutIndex];
		const image_record_t& imageMeta = m_image_meta_table.texture[imageIndex];

		if (clutMeta.num_colors == 256) {
			const int width = imageMeta.width;
			const int height = imageMeta.height;
			const int paletteBuf = m_TEX_PALETTE_BASE + clutMeta.dataOffset;
			const int imageBuf = m_TEX_PALETTE_BASE + imageMeta.dataOffset;
			tex.make_texture(
				(uint8_t*)(buffer + paletteBuf),
				(uint8_t*)(buffer + imageBuf),
				width,
				height,
				csm1ClutIndices);
		}
		else {
			//dbgprint("UNSUPPORTED TEXTURE\n");
		}
	}

	void render(Camera& cam, glm::mat4& matrix) override {
		for (int i = 0; i < m_meshes.size(); i++) {
			m_meshes[i].render(cam, matrix);
		}
	}

	image_meta_table_t const& image_image_table() { return m_image_meta_table; }
	clut_meta_table_t const& clut_meta_table() { return m_clut_meta_table; }
	texture_table_t const& texture_table() { return m_texture_table; }
	std::vector<SlyMesh>& meshes() { return m_meshes; }

private:

	std::vector<SlyMesh> m_meshes;

	int m_TEX_PALETTE_BASE{ 0 };
	uint8_t csm1ClutIndices[256];
	clut_meta_table_t m_clut_meta_table;
	image_meta_table_t m_image_meta_table;
	texture_table_t m_texture_table;
	UNK_TABLE_t unk_table[250];
};

