#version 410

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoords;
layout (location = 3) in int unused;

layout (location = 0) out vec3 o_normal;
layout (location = 1) out vec3 FragPos;
layout (location = 2) out vec2 tex_coord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 additional_model;
uniform mat4 scale;
uniform mat4 rotation;

void main() {
	o_normal = normal;
	vec4 vec4pos = vec4(pos.x, pos.y, pos.z, 1.0);
	FragPos = vec3(rotation * model * vec4pos);
	gl_Position = projection * view * rotation * model * vec4pos;
	//depth = gl_Position.z / 75.0f;
	tex_coord = texcoords;
}