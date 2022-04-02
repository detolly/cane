#pragma once 

#include <Renderer/Object.h>
#include <Utility/ez_stream.h>
#include <Structs/sly_level_structs.h>
#include <Structs/sly_texture_structs.h>

#include <vector>
#include <cstring>

class SlyMesh : public SingleColoredSlyWorldObject {

public:
	texture_table_t& m_texture_table;

	SlyMesh() = delete;
	~SlyMesh();
	SlyMesh(SlyMesh&& o) = default;
	SlyMesh& operator=(SlyMesh && o) = delete;
	SlyMesh(const SlyMesh & o) = delete;
	SlyMesh(std::shared_ptr<mesh_data_t> the_data, std::shared_ptr<mesh_data_t> data_to_render, texture_table_t& texture_table);

	void make_gl_buffers();

    void make_vertex_buffer_gl_buffers();
    void make_szme_buffer_gl_buffers();

	void free_gl_buffers();

	void render(const Camera& cam, const glm::mat4x4& proj) const override;

    mesh_data_t& data() { return *the_data; }
    const mesh_data_t& data() const { return *the_data; }
    const mesh_data_t& data_to_render() const { return *the_data_to_render; }

	std::shared_ptr<mesh_data_t> the_data;
    std::shared_ptr<mesh_data_t> the_data_to_render;

    bool m_is_initialized{ false };
};

class unknown_vector_array : public SingleColoredWorldObject {

    friend class RendererOptions;
public:
    explicit unknown_vector_array(const std::vector<glm::vec3>&& floats) : m_points(std::vector<glm::vec3>(floats)) {}
    explicit unknown_vector_array(unknown_vector_array&& arr)
    {
        m_points = std::move(arr.m_points);
        render_properties = arr.render_properties;
        arr.m_should_delete = false;
    }
    ~unknown_vector_array()
    {
        if (m_should_delete)
            free_gl_buffers();
    }
    void render(const Camera& cam, const glm::mat4& proj) const override;
    void make_gl_buffers();
    void free_gl_buffers();

private:
    enum class draw_function {
        triangles,
        lines,
        points,
        polygon
    } m_draw_func { draw_function::points };

public:
    const draw_function draw_func() const { return m_draw_func; }
    const std::vector<glm::vec3>& points() const { return m_points; }
    bool should_draw() const { return m_should_draw; }
    void set_should_draw(bool b) { m_should_draw = b; }

private:
    struct {
        GLuint vao, vbo;
    } render_properties;

    bool m_should_delete { true };
    std::vector<glm::vec3> m_points;
    bool m_should_draw{ false };
};

class SlyLevelFile : public RenderedWorldObject
{
private:
	SlyLevelFile() = delete;
public:

	SlyLevelFile(const char* level_file);
	~SlyLevelFile();

	//TODO REMOVE DEBUG (or integrate)
	void find_near_float(ez_stream& stream, float x, float y, float z, float allowed_difference);
	void find_and_populate_coord_arrays(ez_stream& stream);

	void parse_meshes(ez_stream& stream);
	void parse_textures(ez_stream& stream);
	void make_texture(const char* buffer, texture_record_t& tex, size_t clutIndex, size_t imageIndex);

	void render(const Camera& cam, const glm::mat4& matrix) const override;

	image_meta_table_t const& image_image_table() { return m_image_meta_table; }
	clut_meta_table_t const& clut_meta_table() { return m_clut_meta_table; }
	texture_table_t const& texture_table() { return m_texture_table; }
    std::vector<std::unique_ptr<SlyMesh>>& meshes() { return m_meshes; }
	std::vector<unknown_vector_array>& unknown_vector_arrays() { return m_unknown_vector_arrays; }

    const image_meta_table_t& image_image_table() const { return m_image_meta_table; }
    const clut_meta_table_t& clut_meta_table() const { return m_clut_meta_table; }
    const texture_table_t& texture_table() const { return m_texture_table; }
    const std::vector<std::unique_ptr<SlyMesh>>& meshes() const { return m_meshes; }
    const std::vector<unknown_vector_array>& unknown_vector_arrays() const { return m_unknown_vector_arrays; }


private:

	std::vector<std::unique_ptr<SlyMesh>> m_meshes;

    std::vector<unknown_vector_array> m_unknown_vector_arrays;
	int m_TEX_PALETTE_BASE{ 0 };
	uint8_t csm1ClutIndices[256];
	clut_meta_table_t m_clut_meta_table;
	image_meta_table_t m_image_meta_table;
	texture_table_t m_texture_table;
	UNK_TABLE_t unk_table[250];
};

