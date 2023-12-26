#include <GL/glew.h>

#include <charconv>
#include <string>

#include <Renderer/Camera.h>
#include <Renderer/OBJModel.h>
#include <Utility/FileReader.h>

OBJModel::OBJModel(std::string_view file_location)
{
    FileReader r(file_location);
    std::string buf = r.read();
    parse(buf);
    make_gl_buffers();
    //det:://dbgprint(buf);
}

void OBJModel::parse(std::string_view obj) {
    int last_start = 0;
    std::vector<glm::vec3> vert_arr;
    for (std::size_t start = 0; start != std::string::npos; last_start = start, start = obj.find("\n", start) + 1) {
        if (obj[last_start] == '#')
            continue;
        const std::string_view current_line = obj.substr(last_start, start-last_start-1);
        if (current_line.starts_with("v "))
        {
            const std::string_view verts = current_line.substr(2);
            const int st1 = verts.find(' ');
            const int st2 = verts.find(' ', st1 + 1);
            const std::string_view vert1 = verts.substr(0, st1);
            const std::string_view vert2 = verts.substr(st1 + 1, st2 - st1);
            const std::string_view vert3 = verts.substr(st2 + 1);

            float v1, v2, v3;
            [[maybe_unused]] const auto result1 = std::from_chars(vert1.begin(), vert1.end(), v1);
            [[maybe_unused]] const auto result2 = std::from_chars(vert2.begin(), vert2.end(), v2);
            [[maybe_unused]] const auto result3 = std::from_chars(vert3.begin(), vert3.end(), v3);

            vert_arr.push_back(glm::vec3(v1, v2, v3));
        }
        else if (current_line.starts_with("f ")) {
            const std::string_view verts = current_line.substr(2);
            const int st1 = verts.find(' ');
            const int st2 = verts.find(' ', st1 + 1);

            const auto index1 = verts.substr(0, st1);
            const auto index2 = verts.substr(st1 + 1, st2 - st1);
            const auto index3 = verts.substr(st2 + 1);

            int i1, i2, i3;
            [[maybe_unused]] const auto result1 = std::from_chars(index1.begin(), index1.end(), i1);
            [[maybe_unused]] const auto result2 = std::from_chars(index2.begin(), index2.end(), i2);
            [[maybe_unused]] const auto result3 = std::from_chars(index3.begin(), index3.end(), i3);

            m_buffer_data.push_back(vert_arr.at(i1-1));
            m_buffer_data.push_back(vert_arr.at(i2-1));
            m_buffer_data.push_back(vert_arr.at(i3-1));
        }
        else if (current_line.starts_with("o ")) {}
        else if (current_line.starts_with("s ")) {}
        else break;
    }
}

void OBJModel::make_gl_buffers()
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glGenBuffers(1, &m_vertex_buffer_id);

    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_id);
    glBufferData(GL_ARRAY_BUFFER, m_buffer_data.size()*sizeof(glm::vec3), m_buffer_data.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
}

void OBJModel::render(const Camera& camera, const glm::mat4& proj) const
{
    shader().set_mvp(game_object().model(), camera.view(), proj);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_buffer_data.size());
}

void OBJModel::free_gl_buffer()
{
    glDeleteBuffers(1, &m_vertex_buffer_id);
    glDeleteVertexArrays(1, &m_vao);
}

