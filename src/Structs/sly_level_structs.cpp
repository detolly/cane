#include "sly_level_structs.h"

#include <Editor.h>
#include <Structs/SlyLevelFile.h>
#include <Utility/dbgprint.h>

mesh_data_t::mesh_data_t(ez_stream& stream, unsigned char field_0x40)
{
    flags = stream.read<uint16_t>();
    if ((~flags) & 1) {
        not_flags_and_1.szms = stream.read<szms_header_t>();
        if (!not_flags_and_1.szms.m_magic.is_szms()) {
            m_error = true;
        }
        std::size_t offset = stream.tell();
        not_flags_and_1.mesh_hdr = mesh_header_t(stream);
        not_flags_and_1.vertex_data.resize(not_flags_and_1.mesh_hdr.mesh_count);

        for (int i = 0; i < not_flags_and_1.mesh_hdr.mesh_count; i++) {
            stream.seek(offset + not_flags_and_1.mesh_hdr.mesh_offsets[i]);
            not_flags_and_1.vertex_data[i] = vertex_data_t(stream, offset);
        }

        magic a = stream.read<::magic>();
        if(!a.is_szme()) {
            m_error = true;
        }

    } else
    {
        flags_and_1.instance_mesh_idx = stream.read<uint16_t>();
        //flags_and_1.instance_mat = stream.read<glm::mat4>();
        float a = stream.read<float>();
        float b = stream.read<float>();
        float c = stream.read<float>();

        float d = stream.read<float>();
        float e = stream.read<float>();
        float f = stream.read<float>();

        float g = stream.read<float>();
        float h = stream.read<float>();
        float i = stream.read<float>();

        float x = stream.read<float>();
        float y = stream.read<float>();
        float z = stream.read<float>();
        flags_and_1.instance_mat = {
                a, b, c, 0.0f,
                d, e, f, 0.0f,
                g, h, i, 0.0f,
                x, y, z, 1.0f
        };
        //flags_and_1.instance_mat = {
        //        a, c, b, 0.0f,
        //        d, f, e, 0.0f,
        //        g, i, h, 0.0f,
        //        x/100.0f, y/100.0f, z/100.0f, 1.0f
        //};
    }
    szme = szme_t(stream, flags, field_0x40);

    //Convert to new code
    //const auto& p = szme.position;

    //dbgprint("%.02f %.02f %.02f\n", p.x, p.y, p.z);

    if (~flags & 1) {

        for(size_t i = 0; i < szme.flags_not_and_1.mesh_count; i++)
            for (size_t j = 0; j < szme.flags_not_and_1.szme_data[i].gl_vertices.size(); j++)
                szme.flags_not_and_1.szme_data[i].gl_vertices[j].pos = (szme.flags_not_and_1.szme_data[i].gl_vertices[j].pos - szme.flags_not_and_1.szme_data[i].origin)/100.0f;

    }

}

szme_vertex_data_t::szme_vertex_data_t(ez_stream& stream, uint16_t flags, unsigned char field_0x40)
{
    (void)flags;
    origin = stream.read_sly_vec();
    unk_float = stream.read<float>();
    vertex_count = stream.read<unsigned char>();
    rotations_count = stream.read<unsigned char>();
    vertex_color_count = stream.read<unsigned char>();
    texcoords_count = stream.read<unsigned char>();
    index_count = stream.read<unsigned char>();

    vertices.resize(vertex_count);
    rotations.resize(rotations_count);
    vertex_colors.resize(vertex_color_count);
    texcoords.resize(texcoords_count);
    indices.resize(index_count);

    //TODO: stream.align(4);
    std::size_t v = stream.tell();
    int pad_size = -v & (3);
    stream.seek(v + pad_size);

    for (int i = 0; i < vertex_count; i++)
        vertices[i] = stream.read_sly_vec();
    for (int i = 0; i < rotations_count; i++)
        rotations[i] = stream.read<normal_t>();
    for (int i = 0; i < vertex_color_count; i++)
        vertex_colors[i] = stream.read<uint32_t>();
    for (int i = 0; i < texcoords_count; i++)
        texcoords[i] = stream.read<texcoord_t>();
    for (int i = 0; i < index_count; i++)
        indices[i] = stream.read<index_t>();

    texture_id = stream.read<uint16_t>();
    unk_u8_1 = stream.read<std::uint8_t>();
    unk_count = stream.read<std::uint8_t>();

    unk_bytes.resize(unk_count);
    for(int i = 0; i < unk_count; i++)
        unk_bytes[i] = stream.read<unsigned char>();

    unk_floats.resize(unk_count * vertex_count);
    for(int i = 0; i < unk_count * vertex_count; i++)
        unk_floats[i] = stream.read<float>();

    if (field_0x40 != 0)
    {
        f_0x40 = field_0x40_data_t(stream, field_0x40, index_count);
    }

    //TODO: handle special case
    gl_vertices.resize(vertices.size());
    for (size_t i = 0; i < vertices.size(); i++) {
        vertex_t vertex;
        vertex.pos = vertices[i];
        gl_vertices[i] = std::move(vertex);
    }
    for(size_t i = 0; i < indices.size(); i++) {
        if (indices[i].vertex_index >= vertices.size())
            continue;
        if (indices[i].normal_index >= rotations.size())
            continue;
        if (indices[i].texcoords_index >= texcoords.size())
            continue;
        triangle_data.push_back(indices[i].vertex_index);
        gl_vertices[indices[i].vertex_index].normal = rotations[indices[i].normal_index];
        gl_vertices[indices[i].vertex_index].tex_coords = texcoords[indices[i].texcoords_index];
    }

    //stream.seek(stream.tell() + unk_count1 * sizeof(vector3_t));
    //stream.seek(stream.tell() + unk_count2 * sizeof(vector3_t));
    //stream.seek(stream.tell() + unk_count3 * sizeof(uint32_t));
    //stream.seek(stream.tell() + unk_count4 * sizeof(vector2_t));
    //stream.seek(stream.tell() + unk_count5 * sizeof(uint32_t));
}

vertex_data_t::vertex_data_t(ez_stream& stream, int mesh_header_start)
{
    vertex_hdr = stream.read<vertex_header_t>();
    vertices.resize(vertex_hdr.vertex_count);
    stream.seek(mesh_header_start + vertex_hdr.vertex_data_offset);
    for (size_t i = 0; i < vertex_hdr.vertex_count; i++) {
        vertices[i] = vertex_t(stream);
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

szms_container::szms_container(ez_stream& stream) {
    field_0x40 = stream.read<unsigned char>();
    if (field_0x40 != 0)
    {
        unk_u32s.resize(field_0x40);
        for(auto i = 0; i < field_0x40; i++)
            unk_u32s[i] = stream.read<uint32_t>();
    }
    szms_count = stream.read<uint16_t>();
    if (szms_count > 2500 || szms_count == 0) {
        m_error = true;
    }
    mesh_datas.resize(szms_count);
    for(auto i = 0; i < szms_count; i++) {
        mesh_datas[i] = std::make_shared<mesh_data_t>(stream, field_0x40);
        if (mesh_datas[i]->m_error) {
            m_error = true;
        }
    }
    flags2 = stream.read<uint32_t>();
    if (flags2 & 1)
        flags2_and_1.unk_mat1 = stream.read<glm::mat3>();
    if (flags2 & 2)
        flags2_and_2.unk_mat2 = stream.read<glm::mat3>();
    if (flags2 & 0xC)
        flags2_and_C.unk_u16 = stream.read<uint16_t>();
    if (flags2 & 0x10)
        flags2_and_10.unk_u16_2 = stream.read<uint16_t>();
    if (flags2 & 0x20)
    {
        flags2_and_20.unk_u16_3 = stream.read<uint16_t>();
        flags2_and_20.unk_vec = stream.read<glm::vec3>();
        flags2_and_20.unk_vec2 = stream.read<glm::vec3>();
        flags2_and_20.unk_float = stream.read<float>();
    }
    if (flags2 & 0x40)
    {
        flags2_and_40.unk_u16_4 = stream.read<uint16_t>();
        flags2_and_40.unk_u16_5 = stream.read<uint16_t>();
    }
    if (flags2 & 0x80)
        flags2_and_80.unk_u8 = stream.read<unsigned char>();

    unk_count = stream.read<unsigned char>();
    unk_structs.resize(unk_count);
    for(int i = 0; i < unk_count; i++)
    {
        unk_structs[i] = unk_t(stream, field_0x40);
    }

    unk_count2 = stream.read<uint16_t>();
}

szms_container::unk_t::unk_t(ez_stream &stream, unsigned char field_0x40)
{
    unk_u16 = stream.read<uint16_t>();
    for(int i = 0; i < field_0x40; i++)
    {
        unk_floats.push_back(stream.read<float>());
    }
}

meta_entry_alt_t::meta_entry_alt_t(ez_stream &stream)
{
    type_maybe = stream.read<uint16_t>();
    if (type_maybe != 0)
        for(auto i = 0; i < 0x1C; i++)
            unk[i] = stream.read<unsigned char>();
}

flags_0x100_t::flags_0x100_t(ez_stream &stream)
{
    unk_u16 = stream.read<uint16_t>();
    unk_u8 = stream.read<unsigned char>();
    if (unk_u8 != (unsigned char)-1)
    {
        for(int i = 0; i < 4; i++)
            u8_not_minus_one.unk_0x100_float_arr[i] = stream.read<float>();
        u8_not_minus_one.mat = stream.read<glm::mat<3, 4, float>>();
    }
    u8_2 = stream.read<unsigned char>();
    u8_3 = stream.read<unsigned char>();
    u8_4 = stream.read<unsigned char>();
}

after_szme_data_nested::after_szme_data_nested(ez_stream& stream, std::uint16_t field_0x40, std::uint8_t unk_count_parent)
{
    unk_count = stream.read<std::uint8_t>();
    unk_vec.resize(unk_count);
    for(auto i = 0; i < unk_count; i++)
        unk_vec.push_back(stream.read<glm::vec3>());

    unk.resize(field_0x40 * unk_count_parent);
    for(auto i = 0; i < field_0x40 * unk_count_parent; i++)
        unk.push_back(stream.read<std::uint16_t>());
}

after_szme_data::after_szme_data(ez_stream& stream, std::uint16_t field_0x40)
{
    unk_count_ = stream.read<uint8_t>();
    unk_vec.resize(unk_count_);
    for(auto i = 0; i < unk_count_; i++)
        unk_vec.push_back(stream.read<glm::vec3>());

    unk_count2 = stream.read<uint8_t>();
    unk_u32.resize(unk_count2);
    for(auto i = 0; i < unk_count2; i++)
        unk_u32.push_back(stream.read<std::uint32_t>());

    unk_count3 = stream.read<uint8_t>();
    unk_u8.resize(unk_count3);
    for(auto i = 0; i < unk_count3; i++)
        unk_u32.push_back(stream.read<std::uint8_t>());

    auto sz = unk_count3 * (std::uint32_t)unk_count_;
    unk_float.resize(sz);
    for(size_t i = 0; i < sz; i++)
        unk_float.push_back(stream.read<float>());

    if (field_0x40 != 0)
        nested_data = after_szme_data_nested(stream, field_0x40, unk_count_);
}

szme_t::szme_t(ez_stream &stream, uint16_t flags, unsigned char field_0x40)
{

    if (flags & 2)
        flags_and_2.unk_0x04 = stream.read<uint32_t>();
    if (flags & 0x200)
        flags_and_200.unk_float = stream.read<float>();
    if (flags & 4)
        flags_and_4.unk_float2 = stream.read<float>();
    if (flags & 8)
        flags_and_8.unk_float3 = stream.read<float>();
    if (flags & 0x10)
        flags_and_10.unk_float4 = stream.read<float>();
    if (flags & 0x20)
        flags_and_20.unk_float5 = stream.read<float>();
    if (flags & 0x40)
        flags_and_40.meta_entry = meta_entry_alt_t(stream);
    if (flags & 0x80)
    {
        flags_and_80.unk_vec3 = stream.read<glm::vec3>();
        flags_and_80.unk_vec4 = stream.read<glm::vec4>();
    }
    if (flags & 0x100)
        flags_and_100.flags_0x100 = flags_0x100_t(stream);

    position = stream.read_sly_vec();
    unk_0x14 = stream.read<float>();
    unk_0x16_ignore = stream.read<uint16_t>();
    unkb1 = stream.read<unsigned char>();
    unkb2 = stream.read<unsigned char>();
    unkb3 = stream.read<unsigned char>();

    if ((~flags) & 1)
    {
        flags_not_and_1.mesh_count = stream.read<uint16_t>();
        if (flags_not_and_1.mesh_count > 2000)
            throw std::runtime_error("let's not");
        flags_not_and_1.szme_data.resize(flags_not_and_1.mesh_count);
        for(int i = 0; i < flags_not_and_1.mesh_count; i++)
        {
            flags_not_and_1.szme_data[i] = szme_vertex_data_t{stream, flags, field_0x40};
        }
        flags_not_and_1.after_szme_data_count = stream.read<uint16_t>();
        flags_not_and_1.after_szme_datas.resize(flags_not_and_1.after_szme_data_count);
        for(auto i = 0; i < flags_not_and_1.after_szme_data_count; i++)
            flags_not_and_1.after_szme_datas[i] = after_szme_data(stream, field_0x40);
    }
}

field_0x40_data_nested_t::field_0x40_data_nested_t(ez_stream &stream, [[maybe_unused]] unsigned char field_0x40, unsigned char index_count)
{
    for(int i = 0; i < index_count; i++)
        unk.emplace_back(stream.read<uint16_t>());
}

field_0x40_data_t::field_0x40_data_t(ez_stream &stream, unsigned char field_0x40, unsigned char index_count)
{
    pos_count = stream.read<uint16_t>();
    positions.resize(pos_count);
    for(int i = 0; i < pos_count; i++)
        positions[i] = stream.read_sly_vec();

    normal_count = stream.read<uint16_t>();
    normals.resize(normal_count);
    for (int i = 0; i < normal_count; ++i)
        normals[i] = stream.read_sly_vec();

    triangle_list.resize(index_count);
    for (int i = 0; i < index_count; ++i)
        triangle_list[i] = stream.read<uint16_t>();

    nested.resize(field_0x40 * 2 - 1);
    for(size_t i = 0; i < nested.size(); i++)
        nested[i] = field_0x40_data_nested_t(stream, field_0x40, index_count);

}
