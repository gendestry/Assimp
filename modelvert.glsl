#version 330 core
layout (location = 0) in vec3 apos;
layout (location = 1) in vec3 anormals;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 norm;

void main() {
	norm = anormals;
	gl_Position = proj * view * model * vec4(apos, 1.0);
}