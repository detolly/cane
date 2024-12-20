#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>

class Camera {

public:

    Camera();
    ~Camera() = default;

    const glm::mat4& view() const { return m_view; }

    glm::vec3 location() const { return m_location; }
    void set_location(glm::vec3 vec) { m_location = vec; m_should_recalculate_view_matrix = true; }

    void calculate_view_matrix_if_needed() { if (m_should_recalculate_view_matrix) calculate_view_matrix(); }

    float pitch() const { return m_pitch; }
    float yaw() const { return m_yaw; } 

    void set_yaw(float yaw) { m_yaw = yaw; m_should_recalculate_view_matrix = true; }
    void set_pitch(float pitch) { m_pitch = pitch; m_should_recalculate_view_matrix = true; }

    void set_yaw_pitch(float yaw, float pitch);

    glm::vec3 direction() const;
    glm::vec3 forward() const;
    glm::vec3 forward_xy() const;
    glm::vec3 left() const;
    glm::vec3 right() const;
    glm::vec3 back() const;
    glm::vec3 back_xy() const;

    constexpr const static glm::vec3 up() { return glm::vec3(0.0f, 1.0f, 0.0f); };

private:

    void calculate_view_matrix();

    bool m_should_recalculate_view_matrix{ true };
    glm::mat4 m_view{ glm::mat4(1.0f) };
    glm::vec3 m_location{ glm::vec3(1.0f) };
    float m_pitch{ 0 }, m_yaw{ -90.f };
};

