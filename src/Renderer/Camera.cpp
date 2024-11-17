#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Renderer/Camera.h>

Camera::Camera()
{
    calculate_view_matrix();
}

glm::vec3 Camera::forward() const
{
    return glm::vec3(direction().x, 0.0f, direction().z);
}

glm::vec3 Camera::left() const
{
    return glm::cross(up(), direction());
}

glm::vec3 Camera::right() const
{
    return -glm::cross(up(), direction());
}

glm::vec3 Camera::back() const
{
    return -forward();
}

glm::vec3 Camera::back_xy() const
{
    return -Camera::forward_xy();
}

glm::vec3 Camera::forward_xy() const
{
    return direction();
}

glm::vec3 Camera::direction() const
{
    return glm::normalize(glm::vec3(glm::cos(glm::radians(yaw())) * glm::cos(glm::radians(pitch())),
                                    glm::sin(glm::radians(pitch())),
                                    glm::sin(glm::radians(yaw())) * glm::cos(glm::radians(pitch()))));
}

void Camera::calculate_view_matrix() {
    m_should_recalculate_view_matrix = false;
    m_view = glm::lookAt(location(), location() + direction(), up());
}

void Camera::set_yaw_pitch(float yaw, float pitch)
{
    float norm = yaw + 180.0f;
    if (norm < 0.0f)
        norm += 360.0f;
    else if (norm > 360.0f)
        norm -= 360.0f;
    m_yaw = norm - 180.0f;
    m_pitch = glm::clamp(pitch, -89.f, 89.f);
    m_should_recalculate_view_matrix = true;
}

