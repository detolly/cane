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
		bool mesh_browser{ false };
	} windows;
	struct {
		bool wireframe { false };
	} renderer;
	struct {

	} debug_information;
};