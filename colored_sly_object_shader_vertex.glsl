#version 410

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoords;
layout (location = 3) in int unused;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

varying float depth;

void main() {
	vec3 new_pos = vec3(-pos.x, pos.z, pos.y);
	gl_Position = projection * view * model * vec4(new_pos, 1.0f);
	depth = gl_Position.z / 75.0f;
}