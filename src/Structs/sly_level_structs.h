#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <Renderer/Object.h>
#include <Utility/ez_stream.h>

namespace {
typedef unsigned char byte;
}

struct texcoord_t {
    float u{0.0f}, v{0.0f};
};

struct normal_t {
    float x{0.0f}, y{0.0f}, z{0.0f};
};

struct magic {
    char magic[4];

    constexpr bool is_szme() const {
        return magic[0] == 'S' && magic[1] == 'Z' && magic[2] == 'M' && magic[3] == 'E';
    }
    constexpr bool is_szms() const {
        return magic[0] == 'S' && magic[1] == 'Z' && magic[2] == 'M' && magic[3] == 'S';
    }
};

struct vertex_t
{
    vertex_t() = default;
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
    NO_COPY(index_data_t);
    index_data_t(ez_stream&, int);

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
    NO_COPY(vertex_data_t);
    vertex_data_t(ez_stream&, int);

    vertex_header_t vertex_hdr;
    std::vector<vertex_t> vertices;
    index_data_t index_hdr;
};

struct szms_header_t
{
    magic m_magic;
    uint32_t version;
    uint32_t data_size;
};

struct mesh_header_t
{
    NO_COPY(mesh_header_t);
    mesh_header_t(ez_stream&);

    uint32_t unknown_0x00;
    uint16_t unknown_0x04;
    uint16_t mesh_count;
    std::vector<uint32_t> mesh_offsets;
};

struct field_0x40_data_nested_t
{
    NO_COPY(field_0x40_data_nested_t);
    field_0x40_data_nested_t(ez_stream&, unsigned char, unsigned char);

    std::vector<uint16_t> unk;
};

struct field_0x40_data_t
{
    NO_COPY(field_0x40_data_t);
    field_0x40_data_t(ez_stream& stream, unsigned char field_0x40, unsigned char index_count);

    uint16_t pos_count;
    uint16_t normal_count;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<uint16_t> triangle_list;
    std::vector<field_0x40_data_nested_t> nested;
};

struct szme_vertex_data_t {
    NO_COPY(szme_vertex_data_t);
    szme_vertex_data_t(ez_stream& stream, uint16_t flags, unsigned char field_0x40);

    static constexpr inline bool is_bad_flags(const uint16_t flags) {
        return flags & 0x40 || flags & 0x4 || flags == 0;
    }

    struct index_t {
        unsigned char vertex_index;
        unsigned char normal_index;
        unsigned char texcoords_index;
        unsigned char unk;
    };

    struct {
        GLuint vao, vbo, ebo;
    } render_properties;

    bool m_initialized{ true };

    glm::vec3 origin;
    float unk_float;
    unsigned char vertex_count;
    unsigned char rotations_count;
    unsigned char vertex_color_count;
    unsigned char texcoords_count;
    unsigned char index_count;

    std::vector<vertex_t> gl_vertices;
    std::vector<uint16_t> triangle_data;

    std::vector<glm::vec3> vertices;
    std::vector<normal_t> normals;
    std::vector<uint32_t> vertex_colors;
    std::vector<texcoord_t> texcoords;
    std::vector<index_t> indices;

    uint16_t texture_id;

    std::uint8_t unk_u8_1;
    std::uint8_t unk_count;
    std::vector<unsigned char> unk_bytes;
    std::vector<float> unk_floats;

    field_0x40_data_t f_0x40;

};

struct flags_0x100_t
{
    NO_COPY(flags_0x100_t);
    flags_0x100_t(ez_stream& stream);

    uint16_t unk_u16;
    unsigned char unk_u8;
    struct {
        float unk_0x100_float_arr[4];
        glm::mat<3, 4, float> mat;
    } u8_not_minus_one;
    unsigned char u8_2;
    unsigned char u8_3;
    unsigned char u8_4;
};

struct meta_entry_alt_t
{
    NO_COPY(meta_entry_alt_t);
    meta_entry_alt_t(ez_stream& stream);

    uint16_t type_maybe;
    unsigned char unk[0x1C];
};

struct after_szme_data_nested
{
    NO_COPY(after_szme_data_nested);
    after_szme_data_nested(ez_stream& stream, std::uint16_t field_0x40, std::uint8_t unk_count_parent);

    std::uint8_t unk_count;
    std::vector<glm::vec3> unk_vec;

    std::vector<std::uint16_t> unk;
};

struct after_szme_data
{
    NO_COPY(after_szme_data);
    after_szme_data(ez_stream& stream, std::uint16_t field_0x40);

    std::uint8_t unk_count_;
    std::vector<glm::vec3> unk_vec;

    std::uint8_t unk_count2;
    std::vector<std::uint32_t> unk_u32;

    std::uint8_t unk_count3;
    std::vector<std::uint8_t> unk_u8;

    std::vector<float> unk_float;

    after_szme_data_nested nested_data;

};

struct szme_t {
    NO_COPY(szme_t);
    szme_t(ez_stream& stream, uint16_t flags, unsigned char field_0x40);

    bool m_error{ false };
    magic m_magic;

    struct {
        uint32_t unk_0x04;
    } flags_and_2;
    struct {
        float unk_float;
    } flags_and_200;
    struct {
        float unk_float2;
    } flags_and_4;
    struct {
        float unk_float3;
    } flags_and_8;
    struct {
        float unk_float4;
    } flags_and_10;
    struct {
        float unk_float5;
    } flags_and_20;
    struct {
        meta_entry_alt_t meta_entry;
    } flags_and_40;
    struct {
        glm::vec3 unk_vec3;
        glm::vec4 unk_vec4;
    } flags_and_80;
    struct {
        flags_0x100_t flags_0x100;
    } flags_and_100;
    struct {
        uint16_t mesh_count;
        std::vector<szme_vertex_data_t> szme_data;
        uint16_t after_szme_data_count;
        std::vector<after_szme_data> after_szme_datas;
        //TODO: IMPLEMENT AFTER_SZME_DATA_T
    } flags_not_and_1;

    glm::vec3 position;
    glm::vec3 raw_position;
    float unk_0x14;
    uint16_t unk_0x16_ignore;
    byte unkb1;
    byte unkb2;
    byte unkb3;

};

struct mesh_data_t
{
    NO_COPY(mesh_data_t);
    mesh_data_t(ez_stream& stream, unsigned char);

    bool m_error{ false };

    struct {
        szms_header_t szms;
        mesh_header_t mesh_hdr;

        struct render_properties {
            GLuint vao, vbo, ebo;
        };

        std::vector<vertex_data_t> vertex_data;
        std::vector<render_properties> render_properties_vector;

        magic m_magic;
    } not_flags_and_1;

    struct {
        uint16_t instance_mesh_idx;
        glm::mat4 instance_mat;
    } flags_and_1;

    szme_t szme;

    uint16_t flags;
};

struct szms_container
{
    NO_COPY(szms_container);
    szms_container(ez_stream& stream);

    bool m_error { false };

    std::uint8_t field_0x40;
    std::vector<uint32_t> unk_u32s;
    uint16_t szms_count;
    std::vector<std::shared_ptr<mesh_data_t>> mesh_datas;
    uint32_t flags2;

    struct {
        glm::mat3 unk_mat1;
    } flags2_and_1;
    struct {
        glm::mat3 unk_mat2;
    } flags2_and_2;
    struct {
        uint16_t unk_u16;
    } flags2_and_C;
    struct {
        uint16_t unk_u16_2;
    } flags2_and_10;
    struct {
        uint16_t unk_u16_3;
        glm::vec3 unk_vec;
        glm::vec3 unk_vec2;
        float unk_float;
    } flags2_and_20;
    struct {
        uint16_t unk_u16_4;
        uint16_t unk_u16_5;
    } flags2_and_40;
    struct {
        std::uint8_t unk_u8;
    } flags2_and_80;

    std::uint8_t unk_count;
    struct unk_t {

        NO_COPY(unk_t);
        unk_t(ez_stream& stream, unsigned char);

        uint16_t unk_u16;
        std::vector<float> unk_floats;
    };
    std::vector<unk_t> unk_structs;

    uint16_t unk_count2;
};
