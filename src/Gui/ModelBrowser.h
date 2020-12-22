#pragma once

#include <glm/mat4x4.hpp>
#include <Renderer/Camera.h>
#include <Gui/RenderBuffer.h>
#include <Utility/Config.h>

class Editor;
class ModelBrowser : public TexturedRenderBuffer {
public:
	ModelBrowser() {
		resize_buffer(width, height); 
		calculate_projection_matrix();
	}

	void render();

	glm::mat4& projection() { if (m_should_recalculate_projection) calculate_projection_matrix(); return m_projection; };
	Camera& camera() { return m_camera; }

private:
	void calculate_projection_matrix() {
		//TODO new fov? idk
		m_projection = glm::perspective(config::the().renderer.fov, (float)width / (float)height, 0.1f, 250.0f);
	}

	Camera m_camera{};
	glm::mat4 m_projection{ 1.0f };

	const int width = 1024;
	const int height = 1024;

	bool m_should_recalculate_projection{ true };

};