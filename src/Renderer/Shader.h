#pragma once

#include <fstream>
#include <unordered_map>
#include <string_view>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class Shader {
public:
	Shader() = delete;
	Shader(const char* vertex_loc, const char* fragment_loc) : m_vs(vertex_loc), m_fs(fragment_loc) {}
	~Shader() = default;

	static void init_shader(Shader& shader) {
		const char* vs = read_from_file(shader.m_vs, &shader.m_vertex_size);
		const char* fs = read_from_file(shader.m_fs, &shader.m_fragment_size);
		make_gl_shader(shader, vs, fs);
		delete[] vs;
		delete[] fs;
	}

	void use();
	inline unsigned int program() { return m_program; }

	void set_vec3(const char* name, const glm::vec3& v);
	void set_vec4(const char* name, const glm::vec4& v);
	void set_mat4(const char* name, const glm::mat4& v);
	void set_mvp(const glm::mat4 model, const glm::mat4 view, const glm::mat4 projection);
	void set_float(const char* name, const float v);

	void insert_location(const char* name);
	unsigned get_location(const char* name);

	const size_t vertex_size() {
		return (size_t)m_vertex_size;
	}

	const size_t fragment_size() {
		return (size_t)m_fragment_size;
	}

private:
	std::streamoff m_vertex_size;
	std::streamoff m_fragment_size;
	unsigned int m_program{0};

	const char* m_vs;
	const char* m_fs;

	std::unordered_map<const char*, unsigned> m_uniform_locations{};

	static void make_gl_shader(Shader& shader, const char* vs, const char* fs);
	static const char* read_from_file(const char* file, std::streamoff* size);


};
