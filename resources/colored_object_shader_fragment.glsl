#version 410

layout (location = 0) out vec4 FragColor;

uniform vec4 col;

varying float depth;

void main() {
	FragColor = col/max(depth, 1.0f);
}