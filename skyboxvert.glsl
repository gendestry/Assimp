#version 330 core
layout (location = 0) in vec3 apos;

uniform mat4 view;
uniform mat4 proj;

out vec3 texCoords;

void main() {
	texCoords = apos;
	vec4 pos = proj * view * vec4(apos, 1.0);
	gl_Position = pos.xyww;
}