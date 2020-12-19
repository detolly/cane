#include <Renderer/Cube.h>
#include <main.h>

Cube::Cube(glm::vec3 location)
{
	game_object().set_location(location);
	make_gl_buffers();
}

Cube::Cube()
{
	make_gl_buffers();
}

Cube::~Cube() {
	free_gl_buffers();
}

void Cube::make_gl_buffers()
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_vertex_buffer_id);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertex_buffer_data), m_vertex_buffer_data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
}

void Cube::free_gl_buffers() {
	//glFreeObjectBufferATI(m_vertex_buffer_id);
	//glFreeObjectBufferATI(m_vao);
}

void Cube::render(Camera& cam, glm::mat4& proj)
{
	SingleColoredWorldObject::render(cam, proj);
	
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, sizeof(m_vertex_buffer_data) / sizeof(float) / 3);
}
