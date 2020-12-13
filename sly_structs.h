#pragma once

#include <cstdint>
#include <Object.h>

#pragma pack(push, 1)

struct texcoord_t {
    float u, v;
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
    index_header_t index_hdr;
    uint16_t*  triangle_data;
    uint16_t*  index_data;
};

struct szme_vertex_t
{
    float pos_x, pos_y, pos_z;
};

struct szme_header_t {
    char magic[4];
    uint32_t unk_0x04;
    float unk_0x08;
    float unk_0x0C;
    float unk_0x10;
    float unk_0x14;
    uint32_t unk_0x18;
    uint32_t unk_0x1C;
    float unk_0x20;
    uint32_t unk_0x24;
    uint32_t unk_0x28;
    unsigned char unk_0x2C[3];
    unsigned char vertex_count;
    unsigned char unk_0x30[3];
    unsigned char unk_count;
};

struct szme_data_t
{
    szme_header_t szme_hdr;
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
    vertex_header_t vertex_hdr;
    vertex_t vertices;
    index_data_t index_data;
};

struct data_header_t
{
    char magic[4];
    uint32_t version;
    uint32_t data_size;
};

struct mesh_header_t
{
    uint32_t unknown_0x00;
    uint16_t unknown_0x04;
    uint16_t mesh_count;
    uint32_t mesh_offsets;
};

struct mesh_data_t
{
    data_header_t header;
    mesh_header_t mesh_hdr;
    vertex_data_t vd;
    szme_data_t szme_data;
};

#pragma pack(pop)