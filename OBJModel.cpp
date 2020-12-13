#include "OBJModel.h"
#include <FileReader.h>
#include <main.h>

OBJModel::OBJModel(const char* file_location)
{
    FileReader r(file_location);
    const char* buf = r.read();
    parse(buf);
    make_gl_buffers();
    //det::dbgprint(buf);
}

void OBJModel::parse(const char* buf) {
    std::string a(buf);
    int last_start = 0;
    std::vector<glm::vec3> vert_arr;
    for (int start = 0; start != std::string::npos; last_start = start, start = a.find("\n", start)+1) {
        if (a[last_start] == '#')
            continue;
        const std::string current_line = a.substr(last_start, start-last_start-1);
        //det::dbgprint(current_line.c_str());
        if (current_line.starts_with("v "))
        {
            const std::string verts = current_line.substr(2);
            const int st1 = verts.find(' ');
            const int st2 = verts.find(' ', st1 + 1);
            const std::string vert1 = verts.substr(0, st1);
            const std::string vert2 = verts.substr(st1 + 1, st2 - st1);
            const std::string vert3 = verts.substr(st2 + 1);
            vert_arr.push_back(glm::vec3((float)std::stof(vert1), (float)std::stof(vert2), (float)std::stof(vert3)));
        }
        else if (current_line.starts_with("f ")) {
            const std::string verts = current_line.substr(2);
            const int st1 = verts.find(' ');
            const int st2 = verts.find(' ', st1 + 1);
            const int index1 = atoi(verts.substr(0, st1).c_str());
            const int index2 = atoi(verts.substr(st1 + 1, st2 - st1).c_str());
            const int index3 = atoi(verts.substr(st2 + 1).c_str());
            m_buffer_data.push_back(vert_arr.at(index1-1));
            m_buffer_data.push_back(vert_arr.at(index2-1));
            m_buffer_data.push_back(vert_arr.at(index3-1));
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

void OBJModel::render(Camera& camera, glm::mat4& proj)
{
    shader().set_mvp(game_object().model(), camera.view(), proj);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_buffer_data.size());
}

void OBJModel::free_gl_buffer()
{

}
