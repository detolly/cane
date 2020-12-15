#pragma once

#include <cstdint>
#include <Object.h>
#include <ez_stream.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#pragma pack(push, 1)

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
    float pos_x, pos_y, pos_z;
    normal_t normal;
    texcoord_t tex_coords;
    uint32_t unk_0x20;

    const vertex_t& operator /= (const float a) {
        pos_x = pos_x / a;
        pos_y = pos_y / a;
        pos_z = pos_z / a;
        return *this;
    }
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
            vertices[i] = stream.read<vertex_t>();
            vertices[i] /= 100.0f;
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
            m.unk_vec3 = stream.read<vector3_t>();
            m.unk_vec4 = stream.read<vector4_t>();
        }
        if (flags & 0x100) {
            //very unsupported
        }
        else {
            m.position = stream.read<vector3_t>();
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
        float unk_float;
        float unk_float2;
        float unk_float3;
        float unk_float4;
        float unk_float5;
        struct {
            vector3_t unk_vec3;
            vector4_t unk_vec4;
        };
        struct {
            vector3_t position;
            float unk_0x14;
            uint16_t unk_0x16_ignore;
            byte unk_0x1A;
            byte unk_0x1B;
            byte unk_0x1C;
            uint16_t mesh_count;
        };
    } m;
};

struct szme_vertex_data_t {
    szme_vertex_data_t() {}
    szme_vertex_data_t(ez_stream& stream) {
        unk_vec = stream.read<vector3_t>();
        unk_float = stream.read<float>();
        unk_count1 = stream.read<unsigned char>();
        unk_count2 = stream.read<unsigned char>();
        unk_count3 = stream.read<unsigned char>();
        unk_count4 = stream.read<unsigned char>();
        unk_count5 = stream.read<unsigned char>();
        pad = stream.read<uint32_t>();
        //for (int i = 0; i < unk_count1; i++)
        //    positions.push_back(stream.read<vector3_t>());
        //for (int i = 0; i < unk_count2; i++)
        //    rotations.push_back(stream.read<vector3_t>());
        //for (int i = 0; i < unk_count3; i++)
        //    unk_color.push_back(stream.read<uint32_t>());
        //for (int i = 0; i < unk_count4; i++)
        //    texcoords.push_back(stream.read<vector2_t>());
        //for (int i = 0; i < unk_count5; i++)
        //    lighting.push_back(stream.read<uint32_t>());
        stream.seek(stream.tell() + unk_count1 * sizeof(vector3_t));
        stream.seek(stream.tell() + unk_count2 * sizeof(vector3_t));
        stream.seek(stream.tell() + unk_count3 * sizeof(uint32_t));
        stream.seek(stream.tell() + unk_count4 * sizeof(vector2_t));
        stream.seek(stream.tell() + unk_count5 * sizeof(uint32_t));
    }

    vector3_t unk_vec;
    float unk_float;
    unsigned char unk_count1;
    unsigned char unk_count2;
    unsigned char unk_count3;
    unsigned char unk_count4;
    unsigned char unk_count5;

    uint32_t pad;

    std::vector<vector3_t> positions;
    std::vector<vector3_t> rotations;
    std::vector<uint32_t> unk_color;
    std::vector<vector2_t> texcoords;
    std::vector<uint32_t> lighting;

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
        }
    }

    struct {
        szms_header_t szms;
        mesh_header_t mesh_hdr;
        szme_header2_t szme_hdr;

        struct {
            GLuint vao, vbo, ebo;
        } render_properties;

        std::vector<vertex_data_t> vertex_data;
        std::vector<szme_vertex_data_t> szme_data;
        std::vector<decltype(render_properties)> render_properties_vector;
    } not_flags_and_1;

    uint16_t flags;
};

#pragma pack(pop)