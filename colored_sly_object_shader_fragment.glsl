#version 410

layout (location = 0) in vec3 normal;
layout (location = 1) in vec3 FragPos;
layout (location = 0) out vec4 FragColor;

uniform vec4 col;

varying float depth;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
	float ambient = 0.15f;
	vec3 lightPos = vec3(0.0f, 100.0f, 0.0f);
	float diff = max(dot(normalize(normal), normalize(lightPos - FragPos)), 0.0f) * (1.f-ambient) + ambient;
	FragColor = (col * diff)/max(depth, 1.0f);
}