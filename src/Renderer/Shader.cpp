#include "Shader.h"

#include <glad/glad.h>
#include <memory>
#include <cstring>
#include <vector>

void Shader::make_gl_shader(Shader& shader, std::string_view vs, std::string_view fs)
{
	GLuint ivs, ifs;
	ivs = glCreateShader(GL_VERTEX_SHADER);
	ifs = glCreateShader(GL_FRAGMENT_SHADER);

    int fs_len = fs.size();
    int vs_len = vs.size();

	glShaderSource(ivs, 1, (const GLchar* const*)&vs, &vs_len);
	glShaderSource(ifs, 1, (const GLchar* const*)&fs, &fs_len);

	glCompileShader(ivs);
	glCompileShader(ifs);

	int  success;
	char infoLog[512];
	glGetShaderiv(ivs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(ivs, 512, NULL, infoLog);
		//dbgprint("ERROR::SHADER::VERTEX::COMPILATION_FAILED\nOBJECT: %x\n%s\n", &shader, infoLog);
	}
	glGetShaderiv(ifs, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(ifs, 512, NULL, infoLog);
		//dbgprint("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\nOBJECT: %x\n%s\n", &shader, infoLog);
	}

	unsigned int program;
	program = glCreateProgram();

	glAttachShader(program, ivs);
	glAttachShader(program, ifs);
	glLinkProgram(program);

    int is{0};
    glGetProgramiv(program, GL_LINK_STATUS, &is);

    if (!is) {
        int maxLength = 5000;
        std::vector<GLchar> infoLog = std::vector<GLchar>(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
    }

	shader.m_program = program;
}

void Shader::use()
{
	glUseProgram(program());
}

void Shader::insert_location(const char* name)
{
	unsigned location = glGetUniformLocation(program(), name);
	m_uniform_locations.insert(std::make_pair<>(name, location));
}

unsigned Shader::get_location(const char* name)
{
	if (!m_uniform_locations.contains(name))
		insert_location(name);
	const auto a = m_uniform_locations.find(name);
	return a != m_uniform_locations.end() ? a->second : 0;
}

void Shader::set_vec3(const char* name, const glm::vec3& v)
{
	unsigned loc = get_location(name);
	use();
	glUniform3fv(loc, 1, (float*)&v[0]);
}

void Shader::set_vec4(const char* name, const glm::vec4& v)
{
	unsigned loc = get_location(name);
	use();
	glUniform4fv(loc, 1, (float*)&v[0]);
}

void Shader::set_mat4(const char* name, const glm::mat4& v)
{
	unsigned loc = get_location(name);
	use();
	glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)&v);
}

void Shader::set_float(const char* name, float v)
{
	unsigned loc = get_location(name);
	use();
	glUniform1f(loc, v);
}

void Shader::set_mvp(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
{
    set_mat4("model", model);
    set_mat4("view", view);
    set_mat4("projection", projection);
}

