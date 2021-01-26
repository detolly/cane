#pragma once 

#include <Renderer/Object.h>
#include <Utility/ez_stream.h>
#include <Structs/sly_level_structs.h>
#include <Structs/sly_texture_structs.h>

#include <vector>

class SlyMesh : public SingleColoredSlyWorldObject {

public:
	texture_table_t& m_texture_table;

	SlyMesh() = delete;
	~SlyMesh() = default;
	SlyMesh(const SlyMesh & o) = default;
	SlyMesh(SlyMesh&& o) = default;
	SlyMesh& operator=(SlyMesh && o) = default;
	SlyMesh(ez_stream& stream, texture_table_t& texture_table);

	void parse(ez_stream& stream);

	void make_gl_buffers();
	void free_gl_buffers();
	void render(Camera& cam, glm::mat4x4& proj) override;

	mesh_data_t mesh_data;
};

struct unknown_vector_array : public SingleColoredWorldObject {

    void make_gl_buffers();
    void free_gl_buffers();

    void render(Camera& cam, glm::mat4& proj) override;

    enum class draw_function {
        triangles,
        lines,
        points
    } draw_func { draw_function::points };
    struct {
        GLuint vao, vbo;
    } render_properties;
    std::vector<float> array;
    bool should_draw{ false };
};

class SlyLevelFile : public RenderedWorldObject
{
private:
	SlyLevelFile() = delete;
public:

	SlyLevelFile(const char* level_file);
	~SlyLevelFile();

	static int find(const char* b, const char* lf, int start, size_t len) {
		int already_found = 0;
		int look_len = std::strlen(lf);
		bool found{ false };
		for (int current_index = 0; (current_index + start) < len; current_index++) {
			const char* current = b + start + current_index - already_found;
			if (b[start + current_index] == lf[already_found])
				already_found++;
			else
				already_found = 0;
			if (already_found == look_len)
				return start + current_index - look_len + 1;
		}
		return -1;
	}

	//TODO REMOVE DEBUG (or integrate)
	void find_near_float(ez_stream& stream, float x, float y, float z, float allowed_difference);
	void find_and_populate_coord_arrays(ez_stream& stream);

	void parse_meshes(ez_stream& stream);
	void parse_textures(ez_stream& stream);
	void make_texture(const char* buffer, texture_record_t& tex, int clutIndex, int imageIndex);

	void render(Camera& cam, glm::mat4& matrix) override;

	image_meta_table_t const& image_image_table() { return m_image_meta_table; }
	clut_meta_table_t const& clut_meta_table() { return m_clut_meta_table; }
	texture_table_t const& texture_table() { return m_texture_table; }
	std::vector<SlyMesh>& meshes() { return m_meshes; }
	std::vector<unknown_vector_array>& unknown_vector_arrays() { return m_unknown_vector_arrays; }

private:

	std::vector<SlyMesh> m_meshes;

    std::vector<unknown_vector_array> m_unknown_vector_arrays;
	int m_TEX_PALETTE_BASE{ 0 };
	uint8_t csm1ClutIndices[256];
	clut_meta_table_t m_clut_meta_table;
	image_meta_table_t m_image_meta_table;
	texture_table_t m_texture_table;
	UNK_TABLE_t unk_table[250];
};

