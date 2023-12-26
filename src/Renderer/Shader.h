#pragma once

#include <fstream>
#include <unordered_map>
#include <string_view>

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <Utility/FileReader.h>

class Shader 
{
public:
	Shader() = delete;
	Shader(std::string_view vertex_loc, std::string_view fragment_loc) : m_vs(vertex_loc), m_fs(fragment_loc) {}
	~Shader() = default;

	void init();

	void use();
	glm::u32 program() const { return m_program; }

	void set_float(const std::string& name, const float v);
	void set_vec3(const std::string& name, glm::vec3 v);
	void set_vec4(const std::string& name, glm::vec4 v);
	void set_mat4(const std::string& name, const glm::mat4& v);
	void set_mvp(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);

	void insert_location(const std::string&);
	glm::u32 get_location(const std::string& name);

	size_t vertex_size() { return static_cast<size_t>(m_vertex_size); }
	size_t fragment_size() { return static_cast<size_t>(m_fragment_size); }

private:
	glm::u32 m_vertex_size;
	glm::u32 m_fragment_size;
	glm::u32 m_program{0};

	std::string m_vs;
	std::string m_fs;

	std::unordered_map<std::string, glm::u32, std::hash<std::string>> m_uniform_locations{};

	void make_gl_shader(const std::string& vs, const std::string& fs);


};

