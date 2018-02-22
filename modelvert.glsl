#version 330 core
layout (location = 0) in vec3 apos;
layout (location = 1) in vec3 anormal;
layout (location = 2) in vec2 atexcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 texcoord;
out vec3 fragPos;
out vec3 norm;

void main() {
	texcoord = atexcoord;
	fragPos = vec3(model * vec4(apos, 1.0));
	norm = vec3(transpose(inverse(model)) * vec4(anormal, 1.0)); // if model * view then the light follows you
	gl_Position = proj * view * model * vec4(apos, 1.0);
}