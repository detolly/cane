#version 410

in vec3 pos;
in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 mvp;

out vec3 onormal;

void main() {
    gl_Position = mvp * vec4(pos, 1.0f);
    onormal = normal;
}
