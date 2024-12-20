#pragma once

#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include <imgui.h>

#include <Gui/RenderBuffer.h>
#include <Renderer/Camera.h>
#include <Utility/config.h>
#include <Utility/Singleton.h>

class Renderer : public TexturedRenderBuffer, public Singleton<Renderer> {
public:
	Renderer() = default;

	void render();
	void handle_input(void* window, float delta_time);
	void on_load();
	void on_close();

	ImVec2 render_location() const { return m_render_location; }
	ImVec2 render_size() const { return m_render_size; }

	glm::mat4& projection() { if (m_should_recalculate_projection) calculate_projection_matrix(); return m_projection; };
	Camera& camera() { return m_camera; }

	int currently_selected_mesh() const { return m_currently_selected_mesh; }
	void select(double x, double y, bool ctrl_modifier = false);

	void set_should_recalculate_projection(bool value) { m_should_recalculate_projection = value; }
	bool should_recalculate_projection() const { return m_should_recalculate_projection; }


private:
	void calculate_projection_matrix();

	ImVec2 m_render_location{}, m_render_size{};

	Camera m_camera{};
	glm::mat4 m_projection{ 1.0f };

	bool m_should_recalculate_projection{ true };
	int m_currently_selected_mesh { -1 };
};

