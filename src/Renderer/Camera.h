#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

class Camera {

public:

	Camera() {
		calculate_view_matrix();
	}
	~Camera() = default;

	const glm::mat4 view() { if (m_should_recalculate_view_matrix) calculate_view_matrix(); return m_view; }
	const glm::vec3 location() const { return m_location; } 
	const glm::vec3& location() { return m_location; }
	void set_location(glm::vec3 vec) { m_location = vec; m_should_recalculate_view_matrix = true; }

	const float pitch() const { return m_pitch; } 
	float pitch() { return m_pitch; }

	const float yaw() const { return m_yaw; } 
	float yaw() { return m_yaw; }

	void set_yaw(float yaw) { m_yaw = yaw; m_should_recalculate_view_matrix = true; }
	void set_pitch(float pitch) { m_pitch = pitch; m_should_recalculate_view_matrix = true; }

	void set_yaw_pitch(float yaw, float pitch) {
		float norm = yaw + 180.0f;
		if (norm < 0.0f)
			norm += 360.0f;
		else if (norm > 360.0f)
			norm -= 360.0f;
		m_yaw = norm - 180.0f;
		m_pitch = glm::clamp(pitch, -89.f, 89.f);
		m_should_recalculate_view_matrix = true;
	}

	const glm::vec3 direction() const;
	const glm::vec3 forward() const;
	const glm::vec3 left() const;
	const glm::vec3 right() const;
	const glm::vec3 back() const;
	const glm::vec3 back_xy() const;
	const static glm::vec3 up();
	const glm::vec3 forward_xy() const;

private:

	void calculate_view_matrix();

	bool m_should_recalculate_view_matrix{ true };
	glm::mat4 m_view{ glm::mat4(1.0f) };
	glm::vec3 m_location{ glm::vec3(1.0f) };
	float m_pitch{ 0 }, m_yaw{ -90.f };
};