#include "Shader.h"

#include <main.h>
#include <memory>

void Shader::make_gl_shader(Shader& shader, const char* vs, const char* fs)
{
	GLuint ivs, ifs;
	ivs = glCreateShader(GL_VERTEX_SHADER);
	ifs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(ivs, 1, (const GLchar* const*)&vs, NULL);
	glShaderSource(ifs, 1, (const GLchar* const*)&fs, NULL);

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

	shader.m_program = program;
}

const char* Shader::read_from_file(const char* file_name, std::streamoff* size = nullptr)
{
	std::ifstream file(file_name, std::ios::in);
	file.seekg(0, std::ios::end);
	std::streamoff len = file.tellg();
	if (size)
		*size = len;
	size_t length = size_t(len);
	char* buf = new char[length]; // not 64-bit compliant - 4GB file size max
	std::memset(buf, 0, length);
	file.seekg(0, std::ios::beg);
	file.read(buf, length);
	//dbgprint("Reading shader from file -- Size: %d bytes\n", len);
	return buf;
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

void Shader::set_mvp(glm::mat4 model, glm::mat4 view, glm::mat4 projection)
{

}