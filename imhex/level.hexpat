import std.mem;
import std.math;
import std.sys;
import std.core;

struct bvec4
{
    u8 x, y, z, w;
};

struct bvec3
{
    u8 x, y, z;
};

struct vec2
{
    float x, y;
};

struct vec3
{
    float x, y, z;
};

struct vec4
{
    float x, y, z, w;
};

struct mat4
{
    vec4 a;
    vec4 b;
    vec4 c;
    vec4 d;
};

struct mat3
{
    vec3 a;
    vec3 b;
    vec3 c;
};

struct mat4x3
{
    vec3 a;
    vec3 b;
    vec3 c;
    vec3 d;
};

struct szms_header
{
    char magic[4];
    if (magic[0] != 'S' || magic[1] != 'Z' || magic[2] != 'M' || magic[3] != 'S') {
        std::print("Failed at 0x{:x}", $);
        break;
    }
    u32 version;
    u32 data_size;
};

struct mesh_header
{
    u32 unknown_0x00;
    u16 unknown_0x04;
    u16 mesh_count;
    u32 mesh_offsets[mesh_count];
};

struct vertex
{
    vec3 pos;
    vec3 normal;
    vec2 texcoord;
    u32 color;
};

struct vertex_header
{
    u16 vertex_count;
    u16 unknown_count;
    u32 vertex_data_offset;
    u32 index_header_offset;

    u32 vertex_data_size = index_header_offset - vertex_data_offset;
    // Sanity-check that all verts are of the size we assume
    u32 vertex_single_size = vertex_data_size / vertex_count;
    std::assert(vertex_single_size == sizeof(vertex), "vertex_single_size vertex_header");
};

struct vertex_data
{
    vertex_header vertex_hdr;
    vertex vertices[vertex_hdr.vertex_count];
    u16 triangle_count;
    u16 index_count;
    u32 index_data_offset_0;
    u32 index_data_offset_1;
    u16 triangle_data[triangle_count * 3];
    u16 index_data[index_count + 1];
};

fn get_indices(auto indices) {
    auto index_array_size = std::core::member_count(indices);
    u32 ret[index_array_size * 3];
    for(u32 i = 0, i < index_array_size, i+=1) {
        ret[i * 3] = indices[i].x;
        ret[i * 3 + 1] = indices[i].y;
        ret[i * 3 + 2] = indices[i].z;
    }
    return ret;
};

struct szme_vertex_data<auto flags, auto unk_container_array_length> {
    vec3 origin;
    float unk_float;
    u8 positions_count;
    u8 normals_count;
    u8 color_count;
    u8 texcoords_count;
    u8 indices_count;

    std::mem::AlignTo<4>;

    vec3 positions[positions_count];
    vec3 normals[normals_count];
    u32 unk_color[color_count];
    vec2 texcoords[texcoords_count];
    bvec4 indices[indices_count];
    
    u16 texture_id;
    u8 unk_flags;
    u8 unk_index_count;
    u8 unk_indexes[unk_index_count];

    if (unk_flags == 70 && unk_index_count == 3)
        float unk_floats[unk_indexes[0] + unk_indexes[1] + unk_indexes[2]];
    else
        float unk_vecs[positions_count * unk_index_count];
   
    /*if(unk_flags > 0) {
        std::print("unk_flags occurrence {:x}, {} {}", $, unk_flags, unk_index_count);
    }
    if(unk_index_count > 0) {
        std::print("unk_index_count occurrence {:x}, {} {}", $, unk_flags, unk_index_count);
    }*/
    
    if (unk_container_array_length > 0) {
        u16 positions_count2;
        vec3 positions2[positions_count2];
        
        u16 normals_count2;
        vec3 normals2[normals_count2];
        
        u16 unk_triangles[indices_count * unk_container_array_length * 2];
    }
} 
//[[hex::visualize("3d", positions, get_indices(indices))]]
;

struct after_szme_data<auto flags> {
    u8 count;
    vec3 unk[count];
    u8 unk_indices_count;
    u32 unk_indices[unk_indices_count];
    if (flags & 8 > 0) {
        u8 unk_array_count;
        u8 unk_array[unk_array_count];
        float unk_vecs[count * unk_array_count];
    } else {
        u8 unk_count;
        u32 unk_array[unk_count];
    }
};

struct szme_t<auto flags, auto unk_container_length, auto after_szme_flags> {
    if ((flags & 2) != 0) {
        u32 unk_0x04;
    }
    if ((flags & 4) != 0) {
        float unk_float2;
    }
    if ((flags & 8) != 0) {
        float unk_float3;
    }
    if ((flags & 16) != 0) {
        float unk_float4;
    }
    if ((flags & 32) != 0) {
        float unk_float5;
    }
    if ((flags & 64) != 0) {
        u16 unk_field_64;
        if (unk_field_64 != 0) {
            u32;
            vec3;
            vec3;
        }
    }
    if ((flags & 128) != 0) {
        vec3 unk_vec3;
        vec4 unk_vec4;
    }
    if ((flags & 256) != 0) {
        break;
    }
    if ((flags & 512) != 0) {
        float unk_float;
    }

    vec3 maybe_origin;    
    float unk_float_2;
    u16 mesh_id;
    u8 almost_always_0;
    u8 almost_always_3;
    u8 almost_always_19;
    
    //std::print("{:6} {} {:08b} {:08b}", flags, almost_always_0, almost_always_3, almost_always_19);
    
    if ((flags & 1) == 0) {
        u16 mesh_count;
        szme_vertex_data<flags, unk_container_length> szme_data[mesh_count];
        
        u16 after_szme_data_length;
        if (after_szme_data_length > 0) {
            std::print("SZME UNK_FLAGS {:x} {} {}", $, after_szme_data_length, after_szme_flags);
        }
        
        after_szme_data<after_szme_flags> after_szme[after_szme_data_length];
    }
    
};

struct mesh<auto unk_container_length, auto after_szme_flags> {

    u16 flags;
    if ((flags & 1) == 0) {
        szms_header szms;
/*
        auto start_offset = $;
        mesh_header mesh_hdr;
        
        vertex_data vertex_data_[mesh_hdr.mesh_count];
*/      
        u8 data[szms.data_size];
        
        /*for (u32 i = 0, i < mesh_hdr.mesh_count, i = i + 1)
        {
            $ = start_offset + mesh_hdr.mesh_offsets[i];
    
            vertex_data vertex_data_(start_offset);
        }*/
        
        char magic[4];
        
        if (magic[0] != 'S' || magic[1] != 'Z' || magic[2] != 'M' || magic[3] != 'E') {
            std::print("Failed SZME at 0x{:x}", $);
            break;
        }
        
    } else {
        u16 instance_mesh_idx;
        mat4x3 instance_mat;
    }
    
    szme_t<flags, unk_container_length, after_szme_flags> szme;
};

fn find_unk_field(auto szme_start) {
    auto actual_start = szme_start - 4;
    for(auto i = 0, i < 0x10, i += 1) {
        if (std::mem::read_unsigned(actual_start - 4 * i - 1, 1) == i
         && std::mem::read_unsigned(actual_start - 4 * i - 6, 2) == 0xFFFF)
            return actual_start - 4 * i - 4;
    }
    std::print("NOTHING FOUND FOR {:x}", szme_start);
    return szme_start - 8;
};

struct mesh_container {
    u8 after_szme_flags;
    u8;
    u8;
    u8 unk_container_array_length;
    u32 unk[unk_container_array_length];
    u16 num_meshes;
    mesh<unk_container_array_length, after_szme_flags> meshes[std::math::max(num_meshes, 1)];
    
    auto current_offset = $;
    auto next_mesh_container = std::mem::find_string_in_range(0, current_offset, 0xFFFFFFFF, "SZMS");
    if (next_mesh_container > 0xFFFFFFFF)
        return;
        
    auto actual_next_start = find_unk_field(next_mesh_container);
    auto padding_count = actual_next_start - current_offset;
    if (padding_count < 0)
        return;
    
    padding[padding_count];
};

struct top_level
{   
};

#pragma pattern_limit 10000000


auto first_mesh_container = std::mem::find_string_in_range(0, 0, 0xFFFFFFFF, "SZMS");
mesh_container containers[10000] @ find_unk_field(first_mesh_container);