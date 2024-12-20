#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL 
#define GLM_ENABLE_EXPERIMENTAL 
#endif

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <Renderer/Shader.h>

class Camera;

class GameObject {
public:
    glm::vec3 scale()       const { return m_scale; }
    glm::vec3 location()    const { return m_location; }
    glm::vec3 rotation()    const { return m_rotation; }

    glm::vec3 raw_scale()    { return m_scale; }
    glm::vec3 raw_location() { return m_location; }
    glm::vec3 raw_rotation() { return m_rotation; }

    void calculate_model_matrix_if_needed();

    bool should_recalculate_model_matrix() const { return m_should_recalculate_model_matrix; }
    void set_should_recalculate_model_matrix(bool should) { m_should_recalculate_model_matrix = should; }

    const glm::mat4& model() const { return m_model; }

    void set_scale(glm::vec3);
    void set_location(glm::vec3);
    void set_rotation(glm::vec4);

    bool constant_model() const { return m_constant_model; }
    void set_constant_model(bool b) { m_constant_model = b; }

    const glm::mat4& constant_model_matrix() const { return m_constant_model_matrix; }
    void set_constant_model_matrix(const glm::mat4 b) { m_constant_model_matrix = b; }

    void move(glm::vec3);

private:
    friend class WorldObject;

    GameObject();

    bool m_constant_model{ false };
    glm::mat4 m_constant_model_matrix{ glm::mat4(1.0f) };

    bool m_should_recalculate_model_matrix{ true };

    glm::mat4 m_model{1.0f};
    glm::vec3 m_scale{1.0f};
    glm::vec3 m_location{0.0f};
    glm::vec3 m_rotation{0.0f};
};

class WorldObject {
public:
    GameObject& game_object() { return m_game_object; };
    const GameObject& game_object() const { return m_game_object; }

private:
    GameObject m_game_object;
};

class SlyObject : public WorldObject
{
public:
    SlyObject() : WorldObject(){}

    static Shader& shader() { return m_shader; }
    virtual void render(const Camera& camera, const glm::mat4& proj) const;
private:
    static Shader m_shader;
    const inline static auto rotation = glm::rotate(glm::identity<glm::mat4>(), glm::radians(3*90.0f), glm::vec3{ 1.0f, 0.0f, 0.0f });
};

