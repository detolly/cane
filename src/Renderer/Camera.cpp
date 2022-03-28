
#include <Renderer/Camera.h>

const glm::vec3 Camera::forward() const {
	return glm::vec3(direction().x, 0.0f, direction().z);
}

const glm::vec3 Camera::left() const {
	return glm::cross(up(), direction());
}

const glm::vec3 Camera::right() const {
	return -glm::cross(up(), direction());
}

const glm::vec3 Camera::back() const {
	return -forward();
}

const glm::vec3 Camera::back_xy() const {
	return -Camera::forward_xy();
}

const glm::vec3 Camera::forward_xy() const {
	return direction();
}

const glm::vec3 Camera::direction() const {
	return glm::normalize(glm::vec3(glm::cos(glm::radians(yaw())) * glm::cos(glm::radians(pitch())), glm::sin(glm::radians(pitch())), glm::sin(glm::radians(yaw())) * glm::cos(glm::radians(pitch()))));
}

void Camera::calculate_view_matrix() {
	m_should_recalculate_view_matrix = false;
	m_view = glm::lookAt(location(), location() + direction(), up());
}

