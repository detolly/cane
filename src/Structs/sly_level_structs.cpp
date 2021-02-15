#include "sly_level_structs.h"
#include <Structs/SlyLevelFile.h>
#include <Editor.h>
#include <Utility/dbgprint.h>

mesh_data_t::mesh_data_t(ez_stream& stream)
{
    flags = stream.read<uint16_t>();
    if (~flags & 1) {
        not_flags_and_1.szms = stream.read<szms_header_t>();
        int offset = stream.tell();
        not_flags_and_1.mesh_hdr = std::move(mesh_header_t(stream));
        not_flags_and_1.vertex_data.resize(not_flags_and_1.mesh_hdr.mesh_count);

        if (not_flags_and_1.mesh_hdr.mesh_count == 0)
            return;

        for (int i = 0; i < not_flags_and_1.mesh_hdr.mesh_count; i++) {
            stream.seek(offset + not_flags_and_1.mesh_hdr.mesh_offsets[i]);
            not_flags_and_1.vertex_data[i] = std::move(vertex_data_t(stream, offset));
        }

        not_flags_and_1.szme_hdr = std::move(szme_header2_t(stream, flags));
        if (~flags & 0x100) {
            not_flags_and_1.szme_data.resize(not_flags_and_1.szme_hdr.m.mesh_count);
            for (int i = 0; i < not_flags_and_1.szme_hdr.m.mesh_count; i++) {
                not_flags_and_1.szme_data[i] = std::move(szme_vertex_data_t(stream, flags));
            }
        }

        const auto& p = not_flags_and_1.szme_hdr.m.position;
        for (uint16_t i = 0; i < not_flags_and_1.mesh_hdr.mesh_count; i++) {
            for (size_t j = 0; j < not_flags_and_1.vertex_data[i].vertices.size(); j++) {
                not_flags_and_1.vertex_data[i].vertices[j].pos = (not_flags_and_1.vertex_data[i].vertices[j].pos - p);
            }
        }
    }
}

szme_vertex_data_t::szme_vertex_data_t(ez_stream& stream, uint16_t flags)
{
    (void)flags;
    unk_vec = stream.read_sly_vec();
    unk_float = stream.read<float>();
    vertex_count = stream.read<unsigned char>();
    normal_count = stream.read<unsigned char>();
    vertex_color_count = stream.read<unsigned char>();
    texcoords_count = stream.read<unsigned char>();
    index_count = stream.read<unsigned char>();

    int v = stream.tell();
    int pad_size = -v & (3);
    stream.seek(v + pad_size);

    vertices.resize(vertex_count);
    normals.resize(normal_count);
    vertex_colors.resize(vertex_color_count);
    texcoords.resize(texcoords_count);
    indices.resize(index_count);
    for (int i = 0; i < vertex_count; i++)
        vertices[i] = stream.read_sly_vec();
    for (int i = 0; i < normal_count; i++)
        normals[i] = stream.read<normal_t>();
    for (int i = 0; i < vertex_color_count; i++)
        vertex_colors[i] = stream.read<uint32_t>();
    for (int i = 0; i < texcoords_count; i++)
        texcoords[i] = stream.read<texcoord_t>();
    for (int i = 0; i < index_count; i++)
        indices[i] = stream.read<index_t>();

    //TODO: handle special case
    try {
        gl_vertices.resize(indices.size());
        for (size_t i = 0; i < indices.size(); i++) {
            vertex_t vertex;
            vertex.pos = vertices.at(indices.at(i).vertex_index);
            vertex.normal = std::move(normals.at(indices.at(i).normal_index));
            vertex.tex_coords = std::move(texcoords.at(indices.at(i).texcoords_index));
            vertex.unk_0x20 = (uint32_t)indices.at(i).unk;
            gl_vertices[i] = std::move(vertex);
        }
    }
    catch (std::out_of_range& e) {
        dbgprint("Failed parsing mesh with flags 0x%04x\nException: %s\n", flags, e.what());
        gl_vertices.clear();
        gl_vertices.resize(0);
        m_initialized = false;
    }

    texture_id = stream.read<uint16_t>();
    unk_u8_1 = stream.read<byte>();
    unk_u8_2 = stream.read<byte>();

    if (m_initialized) {
        game_object().set_constant_model(true);
        set_color({ 1.0f, 0.0f, 0.0f });
        make_gl_buffers();
    }

    //stream.seek(stream.tell() + unk_count1 * sizeof(vector3_t));
    //stream.seek(stream.tell() + unk_count2 * sizeof(vector3_t));
    //stream.seek(stream.tell() + unk_count3 * sizeof(uint32_t));
    //stream.seek(stream.tell() + unk_count4 * sizeof(vector2_t));
    //stream.seek(stream.tell() + unk_count5 * sizeof(uint32_t));
}

void szme_vertex_data_t::make_gl_buffers()
{
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);

    render_properties = { vao, vbo };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, gl_vertices.size() * sizeof(vertex_t), gl_vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)12);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)24);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(vertex_t), (void*)32);
    set_color({ 0.8f, 0.8f, 0.8f });
}

void szme_vertex_data_t::free_gl_buffers()
{
    if (!m_initialized)
        return;

    glDeleteBuffers(1, &render_properties.vbo);
    glDeleteVertexArrays(1, &render_properties.vao);
}

void szme_vertex_data_t::render(Camera& cam, glm::mat4& proj)
{
    if (!m_initialized)
        return;

    SingleColoredWorldObject::render(cam, proj);

    //glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(render_properties.vao);
    if (texture_id < Editor::the().level_file()->texture_table().texture.size())
        glBindTexture(GL_TEXTURE_2D, Editor::the().level_file()->texture_table().texture[texture_id].gl_texture);
    else
        glBindTexture(GL_TEXTURE_2D, 0);

    //glPointSize(8.0f);
    //glLineWidth(8.0f);
    glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, gl_vertices.size());
}

szme_header2_t::szme_header2_t(ez_stream& stream, uint16_t flags)
{
    magic = stream.read<uint32_t>();
    if (flags & 2) {
        m.unk_0x04 = stream.read<uint32_t>();
    }
    if (flags & 0x200) {
        m.unk_float = stream.read<float>();
    }
    if (flags & 4) {
        m.unk_float2 = stream.read<float>();
    }
    if (flags & 8) {
        m.unk_float3 = stream.read<float>();
    }
    if (flags & 0x10) {
        m.unk_float4 = stream.read<float>();
    }
    if (flags & 0x20) {
        m.unk_float5 = stream.read<float>();
    }
    if (flags & 0x40) {
        //unimplemented
        //TODO: handle special case
        stream.seek(stream.tell() + 0x1C);
    }
    if (flags & 0x80) {
        m.unk_vec3 = stream.read<glm::vec3>();
        m.unk_vec4 = stream.read<glm::vec4>();
    }
    if (flags & 0x100) {
        //very unsupported
        //TODO: handle special case
    }
    else {
        m.position = stream.read_sly_vec();
        m.unk_0x14 = stream.read<float>();
        m.unk_0x16_ignore = stream.read<uint16_t>();
        m.unk_0x1A = stream.read<byte>();
        m.unk_0x1B = stream.read<byte>();
        m.unk_0x1C = stream.read<byte>();
        if (~flags & 1)
            m.mesh_count = stream.read<uint16_t>();
    }
}

vertex_data_t::vertex_data_t(ez_stream& stream, int mesh_header_start)
{
    vertex_hdr = stream.read<vertex_header_t>();
    vertices.resize(vertex_hdr.vertex_count);
    stream.seek(mesh_header_start + vertex_hdr.vertex_data_offset);
    for (int i = 0; i < vertex_hdr.vertex_count; i++) {
        vertices[i] = std::move(vertex_t(stream));
    }
    stream.seek(mesh_header_start + vertex_hdr.index_header_offset);
    index_hdr = index_data_t(stream, mesh_header_start);
}

index_data_t::index_data_t(ez_stream& stream, int mesh_header_index)
{
    index_hdr = stream.read<index_header_t>();
    stream.seek(mesh_header_index + index_hdr.index_data_offset_0);
    triangle_data.resize(index_hdr.triangle_count * 3);
    for (int i = 0; i < index_hdr.triangle_count * 3; i++) {
        triangle_data[i] = stream.read<uint16_t>();
    }
    stream.seek(mesh_header_index + index_hdr.index_data_offset_1);
    index_data.resize(index_hdr.index_count);
    for (int i = 0; i < index_hdr.index_count; i++) {
        index_data[i] = stream.read<uint16_t>();
    }
}

mesh_header_t::mesh_header_t(ez_stream& stream)
{
    unknown_0x00 = stream.read<uint32_t>();
    unknown_0x04 = stream.read<uint16_t>();
    mesh_count = stream.read<uint16_t>();
    mesh_offsets.resize(mesh_count);
    for (int i = 0; i < mesh_count; i++) {
        mesh_offsets[i] = stream.read<uint32_t>();
    }
}