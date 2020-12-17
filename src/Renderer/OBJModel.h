#pragma once

#include <Renderer/Object.h>
#include <main.h>
#include <vector>

class OBJModel : public SingleColoredWorldObject {
public:
	OBJModel(const char* file_location);
	~OBJModel() { free_gl_buffer(); }

	virtual void render(Camera& camera, glm::mat4& proj) override;

private:

	void make_gl_buffers();
	void free_gl_buffer();

	void parse(const char* buf);

	GLuint m_vao;
	GLuint m_vertex_buffer_id;
	GLuint m_mvp_loc;

	std::vector<glm::vec3> m_buffer_data;
	size_t m_count{0};
};