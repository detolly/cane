#pragma once

struct config {
private:
	config() = default;
	~config() = default;
	static config c;
public:
	static config& the() { return c; }

	struct {
		bool renderer			{ true };
		bool debug_information	{ true };
		bool model_browser		{ true };
		bool model_viewer		{ true };
		bool examples			{ true };
	} windows;
	struct {
		bool wireframe { false };
		float fov{ 90.0f };
		float move_speed{ 0.01f };
	} renderer;
	struct {

	} debug_information;
	struct {
		struct {
			float r{0.2f}, g{0.2f}, b{0.2f}, a{1.0f};
		} rgba;
	} model_viewer;
};