#version 410

layout (location = 0) in vec3 normal;
layout (location = 1) in vec3 FragPos;
layout (location = 2) in vec2 tex_coord;
layout (location = 0) out vec4 FragColor;

uniform vec4 col;

varying float depth;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform sampler2D tex;

void main() {
	float ambient = 0.25f;
	vec3 lightPos = vec3(0.0f, 1000.0f, 0.0f);
	float diff = abs(dot(normalize(normal), normalize(lightPos - FragPos))) * (1.f-ambient) + ambient;
	//FragColor = (col * diff)/max(depth, 1.0f);
	float thing = diff / max(depth, 1.0f);
	FragColor = texture(tex, tex_coord) * vec4(thing, thing, thing, 1.0f);
}