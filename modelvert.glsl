#version 330 core
layout (location = 0) in vec3 apos;
layout (location = 1) in vec3 anormal;
layout (location = 2) in vec2 atexcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec4 norm;
out vec2 texcoord;

void main() {
	norm = transpose(inverse(model)) * vec4(anormal, 1.0); // if model * view then the light follows you
	texcoord = atexcoord;
	gl_Position = proj * view * model * vec4(apos, 1.0);
}