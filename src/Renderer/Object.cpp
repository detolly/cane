#define GLM_ENABLE_EXPERIMENTAL

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <Renderer/Shader.h>
#include <Renderer/Camera.h>
#include <Renderer/Object.h>

GameObject::GameObject() {}

void GameObject::set_location(const glm::vec3 vec)
{
    m_location = vec;
    m_should_recalculate_model_matrix = true;
}

void GameObject::set_scale(const glm::vec3 scale)
{
    m_scale = scale;
    m_should_recalculate_model_matrix = true;
}

void GameObject::set_rotation(const glm::vec4 rot)
{
    m_rotation = rot;
    m_should_recalculate_model_matrix = true;
}

void GameObject::move(const glm::vec3 vec) {
    GameObject::set_location(m_location + vec);
}

void GameObject::calculate_model_matrix_if_needed() {
    if (should_recalculate_model_matrix()) {
        m_should_recalculate_model_matrix = false;
        //TODO THIS IS COMPLETELY WRONG
        m_model = glm::scale(glm::identity<glm::mat4>(), scale());
        m_model = glm::rotate(m_model, glm::radians(m_rotation.x), glm::vec3(0.0f, 1.0f, 0.0f));
        m_model = glm::rotate(m_model, glm::radians(m_rotation.y), glm::vec3(1.0f, 0.0f, 0.0f));
        m_model = glm::rotate(m_model, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        m_model = glm::translate(m_model, location());
    }
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

void SingleColoredWorldObject::render(const Camera& camera, const glm::mat4& proj) const
{
    shader().use();
    shader().set_vec4("col", m_color);
    shader().set_mat4("model", game_object().constant_model() ? game_object().constant_model_matrix() : game_object().model());
    shader().set_mat4("view", camera.view());
    shader().set_mat4("projection", proj);
}

void SingleColoredWorldObject::set_color(const glm::vec3& color) {
    m_color = glm::vec4(color, 1.0f);
    shader().set_vec4("col", m_color);
}

void SingleColoredSlyWorldObject::render(const Camera& camera, const glm::mat4 & proj) const {
    shader().use();
    shader().set_vec4("col", glm::vec4(color(), 1.0f));
    shader().set_mat4("rotation", rotation);
    shader().set_mat4("model", game_object().constant_model() ? game_object().constant_model_matrix() : game_object().model());
    shader().set_mat4("view", camera.view());
    shader().set_mat4("projection", proj);
}

Shader SingleColoredSlyWorldObject::m_shader{ { Shader("colored_sly_object_shader_vertex.glsl", "colored_sly_object_shader_fragment.glsl") } };

