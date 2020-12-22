#pragma once

struct config {
private:
	config() {}
	static config c;
public:
	static config& the() { return c; }

	struct {
		bool renderer{true};
		bool debug_information{true};
		bool model_browser{ false };
	} windows;
	struct {
		bool wireframe { false };
		float fov{ 90.0f };
		float move_speed{ 0.01f };
	} renderer;
	struct {

	} debug_information;
};