#pragma once

#include <glad/glad.h>
#include <imgui/imgui.h>
#include <Utility/Config.h>
#include <Gui/RenderBuffer.h>
#include <string>
#include <glm/mat4x4.hpp>
#include <imgui/imgui.h>
#include <Renderer/Camera.h>

class RendererWindow : public TexturedRenderBuffer {
public:
	RendererWindow() {}

	void render();
	void handle_input(void* window, float delta_time);
	void on_load();
	void on_close();

	const ImVec2 render_location() const { return m_render_location; }
	const ImVec2 render_size() const { return m_render_size; }

	glm::mat4& projection() { if (m_should_recalculate_projection) calculate_projection_matrix(); return m_projection; };
	Camera& camera() { return m_camera; }

	int currently_selected_mesh() { return m_currently_selected_mesh; }
	void select(double x, double y, bool ctrl_modifier = false);

	void set_should_recalculate_projection(bool value) { m_should_recalculate_projection = value; }
	bool should_recalculate_projection() { return m_should_recalculate_projection; }


private:
	void calculate_projection_matrix();

	ImVec2 m_render_location{}, m_render_size{};

	Camera m_camera{};
	glm::mat4 m_projection{ 1.0f };

	bool m_should_recalculate_projection{ true };
	int m_currently_selected_mesh { -1 };
};