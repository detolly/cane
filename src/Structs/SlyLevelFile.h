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

	SlyMesh(ez_stream& stream, texture_table_t& texture_table) : m_texture_table(texture_table) {
		parse(stream);
		set_color(glm::vec3{
			(float)(stream.tell() / 1 % 255) / 255.0f,
			(float)(stream.tell() / 3 % 255) / 255.0f,
			(float)(stream.tell() / 7 % 255) / 255.0f }
		);
		//if (mesh_data.not_flags_and_1.szme_data.size() > 0)
			//dbgprint("Mesh has texture id of: %d\n", mesh_data.not_flags_and_1.szme_data[0].texture_id);
		make_gl_buffers();
		game_object().set_constant_model(true);
	}

	void parse(ez_stream& stream) {
		//INFINITE LOOP FOR TOMORROW
		mesh_data = std::move(mesh_data_t(stream));
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

	void render(Camera& cam, glm::mat4x4& proj) override
	{
		if (mesh_data.flags & 1)
			return;

		SingleColoredSlyWorldObject::render(cam, proj);

		for (int i = 0; i < mesh_data.not_flags_and_1.mesh_hdr.mesh_count; i++) {
			int t_id = 0;
			if (mesh_data.not_flags_and_1.szme_data.size() >= i+1)
				if (mesh_data.not_flags_and_1.szme_data[i].texture_id < m_texture_table.texture.size())
					if (m_texture_table.texture[mesh_data.not_flags_and_1.szme_data[i].texture_id].is_initialized())
						glBindTexture(GL_TEXTURE_2D, m_texture_table.texture[mesh_data.not_flags_and_1.szme_data[i].texture_id].gl_texture);
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
		m_buffer = reader.read();
		m_buffer_len = reader.length();
		construct();
	}

	~SlyLevelFile() {}

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
		ez_stream stream(m_buffer, m_buffer_len);
		parse_textures(stream);
		parse_meshes(stream);
	}

	void parse_textures(ez_stream& stream)
	{
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

		for (int i = 0; i < 0x100; i += 0x20) {
			for (int j = i; j < i + 8; j++) {
				csm1ClutIndices[j] = j;
				csm1ClutIndices[j + 8] = j + 0x10;
				csm1ClutIndices[j + 0x10] = j + 0x8;
				csm1ClutIndices[j + 0x18] = j + 0x18;
			}
		}

		int offset = detolly::sigscan(
			m_buffer,
			0,
			m_buffer_len,
			" aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80",
			"aaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaax",
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
						make_texture(texture_record, texture_record.clut_index[i], texture_record.image_index[i]);
				}
			}
			else if (is1ImgManyPal) {
				for (int i = 0; i < texture_record.clut_index.size(); i++) {
					if (i == ((texture_record.clut_index.size() / 2)))
						make_texture(texture_record, texture_record.clut_index[i], texture_record.image_index[0]);
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
						make_texture(texture_record, texture_record.clut_index[i], texture_record.image_index[imgIndex]);
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
		while ((current_szme_index = find(m_buffer, "SZMS", current_szme_index + 4, m_buffer_len)) != -1) {
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

	void make_texture(texture_record_t& tex, int clutIndex, int imageIndex) {
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
				(uint8_t*)(m_buffer + paletteBuf),
				(uint8_t*)(m_buffer + imageBuf),
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

private:
	const char* m_buffer{nullptr};
	size_t m_buffer_len{0};

	std::vector<SlyMesh> m_meshes;

	int m_TEX_PALETTE_BASE{ 0 };
	uint8_t csm1ClutIndices[256];
	clut_meta_table_t m_clut_meta_table;
	image_meta_table_t m_image_meta_table;
	texture_table_t m_texture_table;
	UNK_TABLE_t unk_table[250];
};