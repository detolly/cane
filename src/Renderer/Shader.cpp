#include <GL/glew.h>

#include <cstring>
#include <stdexcept>

#include <Renderer/Shader.h>
#include <Utility/dbgprint.h>

void Shader::init()
{
    FileReader reader { m_vs };
    FileReader reader2 { m_fs };
    make_gl_shader(reader.read(), reader2.read());
}

void Shader::make_gl_shader(const std::string& vs, const std::string& fs)
{
    glm::u32 ivs = glCreateShader(GL_VERTEX_SHADER);
    glm::u32 ifs = glCreateShader(GL_FRAGMENT_SHADER);

    int fs_len = fs.size();
    int vs_len = vs.size();

    const GLchar* vertex_shader_array = vs.c_str();
    const GLchar* fragment_shader_array = fs.c_str();
    glShaderSource(ivs, 1, &vertex_shader_array, &vs_len);
    glShaderSource(ifs, 1, &fragment_shader_array, &fs_len);

    glCompileShader(ivs);
    glCompileShader(ifs);

    int  success;
    char infoLog[512];

    glGetShaderiv(ivs, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(ivs, 512, NULL, infoLog);
        printf("%s", infoLog);
        throw std::runtime_error("Could not compile shader");
    }

    glGetShaderiv(ifs, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(ifs, 512, NULL, infoLog);
        printf("%s", infoLog);
        throw std::runtime_error("Could not compile shader");
    }

    unsigned int program;
    program = glCreateProgram();

    glAttachShader(program, ivs);
    glAttachShader(program, ifs);
    glLinkProgram(program);

    int is{0};
    glGetProgramiv(program, GL_LINK_STATUS, &is);

    if (!is) {
        GLint len;
        glGetProgramInfoLog(program, sizeof(infoLog), &len, (GLchar*)infoLog);
        printf("%s", infoLog);
        throw std::runtime_error("Could not link shader");
    }

    m_program = program;
}

void Shader::use()
{
    static ssize_t current_program = -1;
    const auto my_program = program();

    if (current_program == my_program)
        return;

    glUseProgram(my_program);
    current_program = my_program;
}

void Shader::insert_location(const std::string& name)
{
    int location = glGetUniformLocation(program(), name.c_str());
    m_uniform_locations.insert(std::make_pair<>(name, location));
}

glm::u32 Shader::get_location(const std::string& name)
{
    if (!m_uniform_locations.contains(name))
        insert_location(name);
    const auto a = m_uniform_locations.find(name);
    return a != m_uniform_locations.end() ? a->second : 0;
}

void Shader::set_vec3(const std::string& name, const glm::vec3 v)
{
    unsigned loc = get_location(name);
    use();
    glUniform3fv(loc, 1, (float*)&v[0]);
}

void Shader::set_vec4(const std::string& name, const glm::vec4 v)
{
    unsigned loc = get_location(name);
    use();
    glUniform4fv(loc, 1, (float*)&v[0]);
}

void Shader::set_mat4(const std::string& name, const glm::mat4& v)
{
    unsigned loc = get_location(name);
    use();
    glUniformMatrix4fv(loc, 1, GL_FALSE, (float*)&v);
}

void Shader::set_float(const std::string& name, float v)
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

