#pragma once

#include <vector>

#include <Renderer/Object.h>

class OBJModel final : public SingleColoredWorldObject 
{
public:
	OBJModel(std::string_view file_location);
	~OBJModel() { free_gl_buffer(); }

	virtual void render(const Camera& camera, const glm::mat4& proj) const override;

private:

	void make_gl_buffers();
	void free_gl_buffer();

	void parse(std::string_view);

	glm::u32 m_vao;
	glm::u32 m_vertex_buffer_id;

	std::vector<glm::vec3> m_buffer_data;
};

