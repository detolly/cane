#version 410

layout (location = 0) in vec3 pos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

varying float depth;

void main() {
	gl_Position = projection * view * model * vec4(pos, 1.0f);
	depth = gl_Position.z / 75.0f;
}