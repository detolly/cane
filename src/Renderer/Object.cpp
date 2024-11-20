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
    if (!should_recalculate_model_matrix())
        return;

    //TODO THIS IS COMPLETELY WRONG
    m_should_recalculate_model_matrix = false;
    m_model = glm::scale(glm::identity<glm::mat4>(), scale());
    m_model = glm::rotate(m_model, glm::radians(m_rotation.x), glm::vec3(0.0f, 1.0f, 0.0f));
    m_model = glm::rotate(m_model, glm::radians(m_rotation.y), glm::vec3(1.0f, 0.0f, 0.0f));
    m_model = glm::rotate(m_model, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    m_model = glm::translate(m_model, location());
}

void SlyObject::render(const Camera& camera, const glm::mat4 & proj) const {
    shader().use();
    shader().set_mat4("rotation", rotation);
    shader().set_mat4("model", game_object().constant_model() ? game_object().constant_model_matrix() : game_object().model());
    shader().set_mat4("view", camera.view());
    shader().set_mat4("projection", proj);
}

Shader SlyObject::m_shader{ { Shader("sly_object_shader_vertex.glsl", "sly_object_shader_fragment.glsl") } };

