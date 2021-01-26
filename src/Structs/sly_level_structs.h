#pragma once

#include <cstdint>
#include <Renderer/Object.h>
#include <Utility/ez_stream.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <cstddef>
#include <glad/glad.h>

namespace {
typedef unsigned char byte;
}

struct texcoord_t {
    texcoord_t() = default;
    ~texcoord_t() = default;
    texcoord_t(const texcoord_t&) = default;
    texcoord_t(texcoord_t&&) = default;
    texcoord_t& operator=(texcoord_t&& o) = default;
    texcoord_t& operator=(const texcoord_t& o) = default;
    texcoord_t(ez_stream& stream) {
        u = stream.read<float>();
        v = stream.read<float>();
    }
    float u, v;
};

struct normal_t {
    normal_t() {}
    ~normal_t() = default;
    normal_t(normal_t&&) = default;
    normal_t(const normal_t&) = default;
    normal_t& operator=(normal_t&& o) = default;
    normal_t& operator=(const normal_t& o) = default;
    normal_t(ez_stream& stream) {
        x = stream.read<float>();
        y = stream.read<float>();
        z = stream.read<float>();
    }

    float x, y, z;
 };

struct vertex_t
{
    vertex_t() = default;
    ~vertex_t() = default;
    vertex_t(vertex_t&&) = default;
    vertex_t(const vertex_t&) = default;
    vertex_t& operator=(vertex_t&& o) = default;
    vertex_t& operator=(const vertex_t& o) = default;
    vertex_t(ez_stream& stream) {
        pos = stream.read_sly_vec();
        normal = normal_t(stream);
        tex_coords = texcoord_t(stream);
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
    ~index_data_t() = default;
    index_data_t(index_data_t&&) = default;
    index_data_t& operator=(index_data_t&& o) = default;
    index_data_t(ez_stream& stream, int mesh_header_index);

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
    ~vertex_data_t() = default;
    vertex_data_t(vertex_data_t&&) = default;
    vertex_data_t& operator=(vertex_data_t&& o) = default;
    vertex_data_t(ez_stream& stream, int mesh_header_start);

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
    mesh_header_t() = default;
    ~mesh_header_t() = default;
    mesh_header_t(mesh_header_t&&) = default;
    mesh_header_t& operator=(mesh_header_t&& o) = default;
    mesh_header_t(ez_stream& stream);

    uint32_t unknown_0x00;
    uint16_t unknown_0x04;
    uint16_t mesh_count;
    std::vector<uint32_t> mesh_offsets;
};

struct szme_header2_t {

    szme_header2_t() = default;
    ~szme_header2_t() = default;
    szme_header2_t(szme_header2_t&&) = default;
    szme_header2_t& operator=(szme_header2_t&& o) = default;
    szme_header2_t(ez_stream& stream, uint16_t flags);
    
    uint32_t magic;
    struct {
        uint32_t unk_0x04;
        float unk_float {0.0f};
        float unk_float2{0.0f};
        float unk_float3{0.0f};
        float unk_float4{0.0f};
        float unk_float5{0.0f};
        glm::vec3 unk_vec3;
        glm::vec4 unk_vec4;
        glm::vec3 position;
        float unk_0x14;
        uint16_t unk_0x16_ignore;
        byte unk_0x1A;
        byte unk_0x1B;
        byte unk_0x1C;
        uint16_t mesh_count;
    } m;
};

struct szme_vertex_data_t : public SingleColoredSlyWorldObject {
public:
    szme_vertex_data_t() = default;
    ~szme_vertex_data_t() = default;
    szme_vertex_data_t(szme_vertex_data_t&&) = default;
    szme_vertex_data_t& operator=(szme_vertex_data_t&& o) = default;
    szme_vertex_data_t(ez_stream& stream, uint16_t flags);

    void make_gl_buffers();
    void free_gl_buffers();
    void render(Camera& cam, glm::mat4& proj) override;

    inline bool is_bad_flags(uint16_t flags) {
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

    glm::vec3 unk_vec;
    float unk_float;
    unsigned char vertex_count;
    unsigned char normal_count;
    unsigned char vertex_color_count;
    unsigned char texcoords_count;
    unsigned char index_count;

    std::vector<vertex_t> gl_vertices;

    std::vector<glm::vec3> vertices;
    std::vector<normal_t> normals;
    std::vector<uint32_t> vertex_colors;
    std::vector<texcoord_t> texcoords;
    std::vector<index_t> indices;

    uint16_t texture_id;

    byte unk_u8_1;
    byte unk_u8_2;
};

struct mesh_data_t
{
    mesh_data_t() = default;
    ~mesh_data_t() = default;
    mesh_data_t(mesh_data_t&&) = default;
    mesh_data_t& operator=(mesh_data_t&& o) = default;
    mesh_data_t(ez_stream& stream);

    struct {
        szms_header_t szms;
        mesh_header_t mesh_hdr;
        szme_header2_t szme_hdr;

        struct render_properties {
            GLuint vao, ebo, vbo;
        };

        std::vector<vertex_data_t> vertex_data;
        std::vector<szme_vertex_data_t> szme_data;
        std::vector<render_properties> render_properties_vector;
    } not_flags_and_1;

    uint16_t flags;
};