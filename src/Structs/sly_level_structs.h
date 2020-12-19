#pragma once

#include <cstdint>
#include <Renderer/Object.h>
#include <Utility/ez_stream.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <cstddef>

//#pragma pack(push, 1)
#pragma optimize("f", on)

struct RGBA {
    unsigned char r, g, b, a;
};

struct texcoord_t {
    float u, v;
};

struct vector2_t {
    float x;
    float y;
};

struct vector3_t {
    float x;
    float y;
    float z;

    const vector3_t& operator /= (const float a) {
        x = x / a;
        y = y / a;
        z = z / a;
        return *this;
    }
};

struct vector4_t {
    float x;
    float y;
    float z;
    float w;
};

struct normal_t {
    float nx, ny, nz;
};

struct vertex_t
{
    vertex_t() {}
    vertex_t(ez_stream& stream) {
        pos = stream.read_sly_vec();
        normal = stream.read<normal_t>();
        tex_coords = stream.read<texcoord_t>();
        unk_0x20 = stream.read<uint32_t>();
    }

    glm::vec3 pos;
    normal_t normal;
    texcoord_t tex_coords;
    uint32_t unk_0x20;
};

struct index_header_t
{
    uint16_t triangle_count;
    uint16_t index_count;
    uint32_t index_data_offset_0;
    uint32_t index_data_offset_1;
};

struct index_data_t
{
    index_data_t() {}
    index_data_t(ez_stream& stream, int mesh_header_index) {
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

    index_header_t index_hdr;
    std::vector<uint16_t> triangle_data;
    std::vector<uint16_t> index_data;
};

struct vertex_header_t
{
    uint32_t unknown_0x10;
    uint16_t vertex_count;
    uint16_t unknown_coun;
    uint32_t vertex_data_offset;
    uint32_t index_header_offset;
};

struct vertex_data_t
{
    vertex_data_t() {}
    vertex_data_t(ez_stream& stream, int mesh_header_start)
    {
        vertex_hdr = stream.read<vertex_header_t>();
        vertices.resize(vertex_hdr.vertex_count);
        stream.seek(mesh_header_start+vertex_hdr.vertex_data_offset);
        for (int i = 0; i < vertex_hdr.vertex_count; i++) {
            vertices[i] = std::move(vertex_t(stream));
            /* TODO: REMEMBER WHEN EXPORTING */
        }
        stream.seek(mesh_header_start + vertex_hdr.index_header_offset);
        index_hdr = index_data_t(stream, mesh_header_start);
    }

    vertex_header_t vertex_hdr;
    std::vector<vertex_t> vertices;
    index_data_t index_hdr;
};

struct data_header_t
{
    char magic[4];
    uint32_t version;
    uint32_t data_size;
};

struct szms_header_t
{
    char magic[4];
    uint32_t version;
    uint32_t data_size;
};

struct mesh_header_t
{
    mesh_header_t() {}
    mesh_header_t(ez_stream& stream) {
        unknown_0x00 = stream.read<uint32_t>();
        unknown_0x04 = stream.read<uint16_t>();
        mesh_count = stream.read<uint16_t>();
        mesh_offsets.resize(mesh_count);
        for (int i = 0; i < mesh_count; i++) {
            mesh_offsets[i] = stream.read<uint32_t>();
        }
    }

    uint32_t unknown_0x00;
    uint16_t unknown_0x04;
    uint16_t mesh_count;
    std::vector<uint32_t> mesh_offsets;
};

struct szme_header2_t {

    szme_header2_t() {}
    szme_header2_t(ez_stream& stream, uint16_t flags) {
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
            stream.seek(stream.tell() + 0x1C);
        }
        if (flags & 0x80) {
            m.unk_vec3 = stream.read<glm::vec3>();
            m.unk_vec4 = stream.read<glm::vec4>();
        }
        if (flags & 0x100) {
            //very unsupported
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
    
    uint32_t magic;
    struct {
        uint32_t unk_0x04;
        float unk_float {0.0f};
        float unk_float2{0.0f};
        float unk_float3{0.0f};
        float unk_float4{0.0f};
        float unk_float5{0.0f};
        struct {
            glm::vec3 unk_vec3;
            glm::vec4 unk_vec4;
        };
        struct {
            glm::vec3 position;
            float unk_0x14;
            uint16_t unk_0x16_ignore;
            byte unk_0x1A;
            byte unk_0x1B;
            byte unk_0x1C;
            uint16_t mesh_count;
        };
    } m;
};

struct szme_vertex_data_t : public SingleColoredWorldObject {
    szme_vertex_data_t() {}
    szme_vertex_data_t(ez_stream& stream) {
        unk_vec = stream.read_sly_vec();
        unk_float = stream.read<float>();
        position_count = stream.read<unsigned char>();
        rotation_count = stream.read<unsigned char>();
        unk_count3 = stream.read<unsigned char>();
        texcoords_count = stream.read<unsigned char>();
        lighing_count = stream.read<unsigned char>();
        
        int v = stream.tell();
        int pad_size = -v & (3);
        stream.seek(v + pad_size);

        positions.resize(position_count);
        rotations.resize(rotation_count);
        unk_color.resize(unk_count3);
        texcoords.resize(texcoords_count);
        lighting.resize(lighing_count);
        for (int i = 0; i < position_count; i++) {
            //TODO: remember this also
            positions[i] = stream.read_sly_vec();
        }
        for (int i = 0; i < rotation_count; i++)
            rotations[i] = stream.read<glm::vec3>();
        for (int i = 0; i < unk_count3; i++)
            unk_color[i] = stream.read<uint32_t>();
        for (int i = 0; i < texcoords_count; i++)
            texcoords[i] = stream.read<vector2_t>();
        for (int i = 0; i < lighing_count; i++)
            lighting[i] = stream.read<RGBA>();

        game_object().set_constant_model(true);
        set_color({1.0f, 0.0f, 0.0f});
        make_gl_buffers();

        //stream.seek(stream.tell() + unk_count1 * sizeof(vector3_t));
        //stream.seek(stream.tell() + unk_count2 * sizeof(vector3_t));
        //stream.seek(stream.tell() + unk_count3 * sizeof(uint32_t));
        //stream.seek(stream.tell() + unk_count4 * sizeof(vector2_t));
        //stream.seek(stream.tell() + unk_count5 * sizeof(uint32_t));

        texture_id = stream.read<uint16_t>();
        unk_u8_1 = stream.read<byte>();
        unk_u8_2 = stream.read<byte>();
    }

    void make_gl_buffers() {
        GLuint vao, vbo;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glGenBuffers(1, &vbo);

        render_properties = { vao, vbo };

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, positions.size()*sizeof(vector3_t), positions.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
    }

    void render(Camera& cam, glm::mat4& proj) override {
        SingleColoredWorldObject::render(cam, proj);

        glBindVertexArray(render_properties.vao);
        glLineWidth(8.0f);
        glDrawArrays(GL_LINE_STRIP, 0, positions.size());
    }

    struct {
        GLuint vao, vbo;
    } render_properties;

    glm::vec3 unk_vec;
    float unk_float;
    unsigned char position_count;
    unsigned char rotation_count;
    unsigned char unk_count3;
    unsigned char texcoords_count;
    unsigned char lighing_count;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> rotations;
    std::vector<uint32_t> unk_color;
    std::vector<vector2_t> texcoords;
    std::vector<RGBA> lighting;

    uint16_t texture_id;

    byte unk_u8_1;
    byte unk_u8_2;
};

struct mesh_data_t
{
    mesh_data_t() {}
    mesh_data_t(ez_stream& stream) {
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
            if (~flags & 0x100 && ~flags & 1 && not_flags_and_1.mesh_hdr.mesh_count < 0xFF) {
                not_flags_and_1.szme_data.resize(not_flags_and_1.szme_hdr.m.mesh_count);
                for (int i = 0; i < not_flags_and_1.szme_hdr.m.mesh_count; i++) {
                    not_flags_and_1.szme_data[i] = std::move(szme_vertex_data_t(stream));
                }
            }

            const auto& p = not_flags_and_1.szme_hdr.m.position;
            for (int i = 0; i < not_flags_and_1.mesh_hdr.mesh_count; i++) {
                for (int j = 0; j < not_flags_and_1.vertex_data[i].vertices.size(); j++) {
                    not_flags_and_1.vertex_data[i].vertices[j].pos = (not_flags_and_1.vertex_data[i].vertices[j].pos - p);
                }
            }

            if (flags == 2) {

            }
            else if (flags == 0x10 || flags == 0) /* only test no production */ {
                not_flags_and_1.szme_hdr.m.position = not_flags_and_1.szme_data[0].unk_vec;
            }
        }
    }

    struct {
        szms_header_t szms;
        mesh_header_t mesh_hdr;
        szme_header2_t szme_hdr;

        struct render_properties {
            GLuint vao, vbo, ebo;
        };

        std::vector<vertex_data_t> vertex_data;
        std::vector<szme_vertex_data_t> szme_data;
        std::vector<render_properties> render_properties_vector;
    } not_flags_and_1;

    uint16_t flags;
};

#pragma optimize("f", off)