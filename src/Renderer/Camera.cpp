
#include <Renderer/Camera.h>

const glm::vec3 Camera::forward() const {
	glm::vec3 r = glm::vec3(1.0f, 0.0f, 0.0f);
	//r = glm::rotate(r, -m_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	r = glm::rotate(r, -glm::radians(m_yaw) + glm::pi<float>() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	return r;
}

const glm::vec3 Camera::left() const {
	glm::vec3 r = glm::vec3(1.0f, 0.0f, 0.0f);
	//r = glm::rotate(r, -m_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	r = glm::rotate(r, -glm::radians(m_yaw) + glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
	return r;
}

const glm::vec3 Camera::right() const {
	glm::vec3 r = glm::vec3(1.0f, 0.0f, 0.0f);
	//r = glm::rotate(r, -m_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	r = glm::rotate(r, -glm::radians(m_yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	return r;
}

const glm::vec3 Camera::back() const {
	glm::vec3 r = glm::vec3(1.0f, 0.0f, 0.0f);
	//r = glm::rotate(r, -m_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	r = glm::rotate(r, -glm::radians(m_yaw) - glm::pi<float>() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	return r;
}

const glm::vec3 Camera::back_xy() const {
	return -Camera::forward_xy();
}

const glm::vec3 Camera::forward_xy() const {
	//dbgprint("not tested -- Camera::forward_xy();\n");
	glm::vec3 r = glm::vec3(1.0f, 0.0f, 0.0f);
	r = glm::rotate(r, -glm::radians(m_yaw) + glm::pi<float>() / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//r = glm::rotate(r, -glm::radians(m_pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	return r;
}

const glm::vec3 Camera::up() const {
	return glm::vec3(0.0f, 1.0f, 0.0f);
}

void Camera::calculate_view_matrix() {
	m_should_recalculate_view_matrix = false;
	m_view = glm::mat4(1.0f);
	m_view = glm::rotate(m_view, glm::radians(pitch()), glm::vec3(1.0f, 0.0f, 0.0f));
	m_view = glm::rotate(m_view, glm::radians(yaw()), glm::vec3(0.0f, 1.0f, 0.0f));
	m_view = glm::translate(m_view, -m_location);
}