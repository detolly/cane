#include <Object.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <Shader.h>
#include <Camera.h>

GameObject::GameObject() {}

void GameObject::set_location(const glm::vec3& vec)
{
	m_location = vec;
	m_should_recalculate_model_matrix = true;
}

const glm::mat4& GameObject::model()
{
	if (should_recalculate_model_matrix()) {
		m_should_recalculate_model_matrix = false;
		m_model = glm::scale(glm::mat4(1.0f), scale());
		m_model = glm::translate(m_model, location());
		//TODO: rotate
	}
	return m_model;
}

void GameObject::set_scale(const glm::vec3& scale)
{
	m_scale = scale;
	m_should_recalculate_model_matrix = true;
}

void GameObject::set_rotation(const glm::vec4& rot)
{
	m_rotation = rot;
	m_should_recalculate_model_matrix = true;
}

void GameObject::move(const glm::vec3& vec) {
	GameObject::set_location(m_location + vec);
}

Shader SingleColoredWorldObject::m_shader{ { Shader("colored_object_shader_vertex.glsl", "colored_object_shader_fragment.glsl") } };

SingleColoredWorldObject::SingleColoredWorldObject()
{
	glm::vec3 color{ 1.0f };
	set_color(color);
}

SingleColoredWorldObject::SingleColoredWorldObject(glm::vec3 color) :
	m_color(glm::vec4(color, 1.0f))
{
	set_color(color);
}

void SingleColoredWorldObject::render(Camera& camera, glm::mat4& proj)
{
	shader().use();
	shader().set_vec4("col", m_color);
	shader().set_mat4("model", game_object().constant_model() ? game_object().constant_model_matrix() : game_object().model());
	shader().set_mat4("view", camera.view());
	shader().set_mat4("projection", proj);
}

void SingleColoredWorldObject::set_color(glm::vec3 color) {
	m_color = glm::vec4(color, 1.0f);
	shader().set_vec4("col", m_color);
}