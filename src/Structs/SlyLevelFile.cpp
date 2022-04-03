#include "SlyLevelFile.h"

#include <Renderer/Camera.h>
#include <memory>

#include <Utility/dbgprint.h>
#include <Utility/Sigscan.h>
#include <Utility/FileReader.h>

#include <Editor.h>

#ifdef WIN32
#include <windows.h>
#endif

SlyMesh::SlyMesh(std::shared_ptr<mesh_data_t> the_mesh_data, std::shared_ptr<mesh_data_t> data_to_render, texture_table_t& texture_table)
	: m_texture_table(texture_table), the_data(the_mesh_data), the_data_to_render(data_to_render)
{
	//set_color(glm::vec3{
	//	(float)(stream.tell() / 1 % 255) / 255.0f,
	//	(float)(stream.tell() / 3 % 255) / 255.0f,
	//	(float)(stream.tell() / 7 % 255) / 255.0f }
	//);
	set_color({ 0.8f, 0.8f, 0.8f });
    //game_object().set_scale(glm::vec3{ 0.01f, 0.01f, 0.01f });
    if (data().flags & 1) {

        if (data().flags_and_1.instance_mesh_idx == 0) {
            game_object().set_constant_model(false);
            game_object().set_location(data().szme.position/100.0f);
            game_object().calculate_model_matrix_if_needed();
        } else {

            //const auto new_pos = data().flags_and_1.instance_mat * glm::vec4 { data().szme.position, 1.0f };
            //const auto new_new_pos = ez_stream::to_sly_vector(new_pos);
            //game_object().set_location(new_new_pos);
            //game_object().calculate_model_matrix_if_needed();

            //const static auto rot = glm::rotate(glm::identity<glm::mat4>(), glm::radians(90.0f*3), glm::vec3{ 1.0f, 0.0f, 0.0f });
            //set_additional_model_matrix(data().flags_and_1.instance_mat);
            game_object().set_location(data().szme.position/100.0f);
            game_object().calculate_model_matrix_if_needed();
        }

    } else {
        game_object().set_constant_model(false);
        game_object().set_location(data().szme.position/100.0f);
        game_object().calculate_model_matrix_if_needed();
    }
}

void SlyMesh::make_gl_buffers()
{
    //make_szme_buffer_gl_buffers();
    make_vertex_buffer_gl_buffers();
}

void SlyMesh::make_szme_buffer_gl_buffers()
{
    for(auto& szme_fragment : data().szme.flags_not_and_1.szme_data) {
        if (!szme_fragment.m_initialized)
            continue;
        GLuint VAO, VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        szme_fragment.render_properties = { VAO, VBO, EBO };

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,
                     szme_fragment.gl_vertices.size() * sizeof(vertex_t),
                     szme_fragment.gl_vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     szme_fragment.triangle_data.size() * sizeof(uint16_t),
                     szme_fragment.triangle_data.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) 12);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) 24);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(vertex_t), (void *) 32);

        glBindVertexArray(0);
        set_color({ 0.8f, 0.8f, 0.8f });
    }
}

void SlyMesh::make_vertex_buffer_gl_buffers()
{
    if (~data().flags & 1) {
        data().not_flags_and_1.render_properties_vector.resize(data().not_flags_and_1.mesh_hdr.mesh_count);
        if (data().not_flags_and_1.mesh_hdr.mesh_count != data().not_flags_and_1.vertex_data.size())
            return;
        gl_vertices.resize(data().not_flags_and_1.mesh_hdr.mesh_count);
        for (int i = 0; i < data().not_flags_and_1.mesh_hdr.mesh_count; i++) {
            GLuint VAO, VBO, EBO;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            glBindVertexArray(VAO);

            gl_vertices[i] = data().not_flags_and_1.vertex_data[i].vertices;

            for (auto& vertex : gl_vertices[i])
                vertex.pos = (vertex.pos - data().szme.position)/100.0f;

            data().not_flags_and_1.render_properties_vector[i] = { VAO, VBO, EBO };

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER,
                         gl_vertices[i].size() * sizeof(vertex_t),
                         gl_vertices[i].data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         data().not_flags_and_1.vertex_data[i].index_hdr.triangle_data.size() * sizeof(uint16_t),
                         data().not_flags_and_1.vertex_data[i].index_hdr.triangle_data.data(), GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) 0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) 12);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) 24);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(vertex_t), (void *) 32);

            glBindVertexArray(0);
        }
    } else {
        if (data_to_render().flags & 1)
            return;
        data().not_flags_and_1.render_properties_vector.resize(data_to_render().not_flags_and_1.mesh_hdr.mesh_count);
        if (data_to_render().not_flags_and_1.mesh_hdr.mesh_count != data_to_render().not_flags_and_1.vertex_data.size())
            return;
        gl_vertices.resize(data_to_render().not_flags_and_1.mesh_hdr.mesh_count);
        for (int i = 0; i < data_to_render().not_flags_and_1.mesh_hdr.mesh_count; i++) {
            GLuint VAO, VBO, EBO;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            glBindVertexArray(VAO);

            const auto translate_vertices = [this](const std::vector<vertex_t>& vertices) {
                std::vector<vertex_t> new_vertices;
                new_vertices.resize(vertices.size());
                int i = 0;
                for(const auto& vertex : vertices) {
                    auto new_vertex = vertex;
                    new_vertex.pos = ((data().flags_and_1.instance_mat * glm::vec4{ vertex.pos, 1.0f }) - glm::vec4{ data().szme.position, 1.0f })/100.0f;
                    new_vertices[i] = new_vertex;
                    i++;
                }
                return new_vertices;
            };

            gl_vertices[i] = translate_vertices(data_to_render().not_flags_and_1.vertex_data[i].vertices);

            data().not_flags_and_1.render_properties_vector[i] = { VAO, VBO, EBO };

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, gl_vertices[i].size() * sizeof(vertex_t), gl_vertices[i].data(), GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         data_to_render().not_flags_and_1.vertex_data[i].index_hdr.triangle_data.size() * sizeof(uint16_t),
                         data_to_render().not_flags_and_1.vertex_data[i].index_hdr.triangle_data.data(), GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) 0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) 12);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void *) 24);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(vertex_t), (void *) 32);

            glBindVertexArray(0);
        }
    }
    m_is_initialized = true;
}

void SlyMesh::free_gl_buffers()
{
    //for (const auto &szme_fragment: data.szme.flags_not_and_1.szme_data) {
    //    if (!szme_fragment.m_initialized)
    //        return;
    //    glDeleteBuffers(1, &szme_fragment.render_properties.ebo);
    //    glDeleteBuffers(1, &szme_fragment.render_properties.vbo);
    //    glDeleteVertexArrays(1, &szme_fragment.render_properties.vao);
    //}

    for (const auto &rp: data().not_flags_and_1.render_properties_vector) {
        glDeleteBuffers(1, &rp.ebo);
        glDeleteBuffers(1, &rp.vbo);
        glDeleteVertexArrays(1, &rp.vao);
    }
}

SlyMesh::~SlyMesh() noexcept
{
    free_gl_buffers();
}

void SlyMesh::render(const Camera& cam, const glm::mat4x4& proj) const
{
    if (!m_is_initialized)
        return;

    if (data().flags == 0)
        return;

    SingleColoredSlyWorldObject::render(cam, proj);

    if (data_to_render().flags & 1)
        return;

    for (size_t i = 0; i < data_to_render().not_flags_and_1.mesh_hdr.mesh_count; i++) {
        if (data_to_render().szme.flags_not_and_1.szme_data.size() > i &&
                data_to_render().szme.flags_not_and_1.szme_data[i].texture_id < m_texture_table.texture.size() &&
            m_texture_table.texture[data_to_render().szme.flags_not_and_1.szme_data[i].texture_id].is_initialized())
            glBindTexture(GL_TEXTURE_2D,
                          m_texture_table.texture[data_to_render().szme.flags_not_and_1.szme_data[i].texture_id].gl_texture);
        else
            glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(data().not_flags_and_1.render_properties_vector[i].vao);
        glDrawElements(GL_TRIANGLES, data_to_render().not_flags_and_1.vertex_data[i].index_hdr.triangle_data.size(),
                       GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }
}

SlyLevelFile::SlyLevelFile(const char* level_file)
{
	FileReader reader(level_file);
	const auto* bf = reader.read();
	const auto sz = reader.length();
	ez_stream stream(bf, sz);
	parse_textures(stream);
	parse_meshes(stream);
	//find_and_populate_coord_arrays(stream);
}

SlyLevelFile::~SlyLevelFile()
{
	for (auto& tex_record : m_texture_table.texture) {
		glDeleteTextures(1, &tex_record.gl_texture);
	}
}

void SlyLevelFile::find_near_float(ez_stream& stream, float x, float y, float z, float allowed_difference)
{
	for (size_t i = 0; i < stream.size(); i++) {
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

void SlyLevelFile::parse_textures(ez_stream& stream)
{
	size_t finding_texture_table = 0;
	int s = 0;
    stream.seek(0);
	while (stream.find("FK$Dcrmtaunt07")) {
        finding_texture_table = stream.tell();
        stream.read<int>();
    }
	finding_texture_table += 33;
	stream.seek(finding_texture_table);
	m_clut_meta_table = clut_meta_table_t(stream);
	m_image_meta_table = image_meta_table_t(stream);
	m_texture_table = texture_table_t(stream);
	for (int i = 0; i < 250; i++) {
		unk_table[i] = stream.read<UNK_TABLE_t>();
	}

	for (uint16_t i = 0; i < 0x100; i += 0x20) {
		for (uint16_t j = i; j < i + 8; j++) {
			csm1ClutIndices[j] = static_cast<uint8_t>(j);
			csm1ClutIndices[j + 8] = static_cast<uint8_t>(j) + 0x10;
			csm1ClutIndices[j + 0x10] = static_cast<uint8_t>(j) + 0x8;
			csm1ClutIndices[j + 0x18] = static_cast<uint8_t>(j) + 0x18;
		}
	}

	int offset = detolly::sigscan(
		stream.buffer(),
		0,
		stream.size(),
		" aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80"
        " aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80"
        " aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80"
        " aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80 aa\x80"
        " aa\x80 aa\x80 aa\x80 aa\x80",
        "aaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaax"
        "aaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaax"
        "aaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaaxaaax",
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
			for (size_t i = 0; i < texture_record.clut_index.size(); i++) {
				if (i == ((texture_record.clut_index.size() / 2)))
					make_texture(stream.buffer(), texture_record, texture_record.clut_index[i], texture_record.image_index[i]);
			}
		}
		else if (is1ImgManyPal) {
			for (size_t i = 0; i < texture_record.clut_index.size(); i++) {
				if (i == ((texture_record.clut_index.size() / 2)))
					make_texture(stream.buffer(), texture_record, texture_record.clut_index[i], texture_record.image_index[0]);
			}
		}
		else if (isManyImgManyPal) {
			//if (!Number.isInteger(texEntry.clutIndices.length / texEntry.imageIndices.length)) {
			//	console.log(`WARN: nonint m2m ${ texEntryIdx } ${ texEntry.clutIndices.length } ${ texEntry.imageIndices.length }`);
			//}
			const auto divPalImg = texture_record.clut_index.size() / texture_record.image_index.size();
			for (size_t i = 0; i < texture_record.clut_index.size(); ++i) {
				int imgIndex = i / (divPalImg == 0 ? 1 : divPalImg);
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

void SlyLevelFile::render(const Camera& cam, const glm::mat4& matrix) const
{
	for (size_t i = 0; i < m_meshes.size(); i++) {
		m_meshes[i]->render(cam, matrix);
	}
	for(size_t i = 0; i < unknown_vector_arrays().size(); i++) {
	    if (unknown_vector_arrays()[i].should_draw())
	        unknown_vector_arrays()[i].render(cam, matrix);
	}
}

void SlyLevelFile::parse_meshes(ez_stream& stream)
{
    stream.seek(0);
    std::string_view looking_for = { "SZMS\x04\x00\x00\x00", 8 };
    //std::string_view looking_for = { "SZMS", 4 };
#if 1
	int total = 0;
	size_t current_szme_index{ 0 };
	while (stream.find(looking_for)) {

        current_szme_index = stream.tell()+1;
	    uint64_t field_0x40 = 0;

        bool found = false;
        for (auto j = 0; j < 0xB; j++) {
            if ((stream.read_at<uint16_t>(current_szme_index - 4 - 6 - j * 4) == 0xFFFF) &&
                ((stream.read_at<uint8_t> (current_szme_index - 4 - 4 - j * 4) == 0x01) ||
                 (stream.read_at<uint8_t> (current_szme_index - 4 - 4 - j * 4) == 0x00))) {
                if (stream.read_at<uint8_t>(current_szme_index - 4 - 1 - j * 4) == j) {
                    field_0x40 = j;
                    found = true;
                }
                break;
            }
        }
        if (!found) {
            dbgprint("[0x%x] not found!\n", current_szme_index);
        }

        size_t szms_start_real = current_szme_index - 4 - (4 * field_0x40 + 1);

		stream.seek(szms_start_real);

		total++;
		dbgprint("Found another object.. Total: %d\r\n", total);

        szms_container container = { stream };

        std::vector<std::shared_ptr<mesh_data_t>> references;
        std::vector<SlyMesh*> added_now;
        int i = 0;
        for(auto& mesh_data_ptr : container.mesh_datas) {
            std::shared_ptr<mesh_data_t> to_render{ mesh_data_ptr };

            references.push_back(mesh_data_ptr);
            m_meshes.emplace_back(std::make_unique<SlyMesh>(mesh_data_ptr, to_render, m_texture_table));

            added_now.push_back(m_meshes.back().get());

            i++;
        }
        for(auto* mesh_ptr : added_now)
        {
            auto& mesh = *mesh_ptr;
            if (mesh.data().flags & 1 && mesh.the_data_to_render.get() == mesh.the_data.get()) {
                if (mesh.data().flags_and_1.instance_mesh_idx < references.size())
                    mesh.the_data_to_render = references.at(mesh.data().flags_and_1.instance_mesh_idx);
                else
                    dbgprint("Found a weird ass mesh\n");
            }
            mesh.make_gl_buffers();
        }
	}
#else
    int total = 0;
    std::size_t current_szme_index{ 0 };
    std::size_t szms_start{ 0 };
    std::size_t szms_start_real{ 0 };

    const auto get_field0x40 = [](int idx) {
        switch (idx) {
            case 83: return 1;
            case 159: return 8;
            case 185: return 4;
            case 213: return 2;
            case 296: return 10;
            case 299: return 4;
            case 300: return 5;
            default: return 0;
        }
    };

    while ((szms_start = find(stream.buffer(), looking_for, szms_start + 4, stream.size())) != -1) {
        szms_start_real = szms_start - 4 - (4 * get_field0x40(total) + 1);

        stream.seek(szms_start_real);
        try {
            szms_container container = szms_container(stream);
            if (container.szms_count > 0) {
                for(auto& mesh_data : container.mesh_datas)
                    m_meshes.emplace_back(std::move(mesh_data), m_texture_table);
            }
        } catch (std::exception& e)
        {
            dbgprint("%s\n", e.what());
        }
        total++;
    }
#endif
}

void SlyLevelFile::make_texture(const char* buffer, texture_record_t& tex, size_t clutIndex, size_t imageIndex)
{
	if (clutIndex >= m_clut_meta_table.record.size()) {
		//dbgprint("warn: clutIndex(%d) out of bounds, skipping\n", clutIndex);
		return;
	}
	if (imageIndex >= static_cast<size_t>(m_image_meta_table.header.numRecords)) {
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

void SlyLevelFile::find_and_populate_coord_arrays(ez_stream& stream)
{
    stream.seek(0);

    const auto is_valid_vector = [](const glm::vec3& vec) -> bool {
        const auto is_valid_float = [](const float& f, const bool is_y=false) -> bool {
            if (!is_y)
                return  -500.0f < f && f < 500.0f && (f > 1.0f || f < -1.0f);
            return  -100.0f < f && f < 100.0f && (f > 0.1f || f < -0.1f);
        };
        return is_valid_float(vec.x) && is_valid_float(vec.y, true) && is_valid_float(vec.z);
    };

    while(stream.tell() < stream.size() - sizeof(glm::vec3)) {
        std::vector<glm::vec3> unknowns;
        const auto before = stream.tell();
        for (auto vec = stream.read_sly_vec(); is_valid_vector(vec); vec = stream.read_sly_vec()) {
            unknowns.push_back(vec);
        }

        if (unknowns.size() > 3) {
            unknown_vector_arrays().emplace_back(std::move(unknowns));
            unknown_vector_arrays().back().set_color({(rand() % 256) / 255.0f, (rand() % 256) / 255.0f, (rand() % 256) / 255.0f});
            unknown_vector_arrays().back().make_gl_buffers();
            continue;
        }

        stream.seek(before+1);
    }
}

void unknown_vector_array::free_gl_buffers()
{
    glDeleteBuffers(1, &render_properties.vbo);
    glDeleteBuffers(1, &render_properties.vao);
}

void unknown_vector_array::make_gl_buffers()
{
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);

    render_properties = { vao, vbo };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, points().size() *  3 * sizeof(float), points().data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
}

void unknown_vector_array::render(const Camera &cam, const glm::mat4 &proj) const
{
    SingleColoredWorldObject::render(cam, proj);
    glBindVertexArray(render_properties.vao);
    switch(draw_func()) {
        case draw_function::lines:
            glDrawArrays(GL_LINES, 0, points().size()-1);
            break;
        case draw_function::points:
            glPointSize(4);
            glDrawArrays(GL_POINTS, 0, points().size());
            break;
        case draw_function::triangles:
            glDrawArrays(GL_TRIANGLES, 0, points().size());
            break;
        case draw_function::polygon:
            glDrawArrays(GL_POLYGON, 0, points().size());
    }
}
