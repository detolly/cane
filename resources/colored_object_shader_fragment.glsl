#version 410

layout (location = 0) out vec4 FragColor;

uniform vec4 col;

void main() {
	FragColor = col;
}