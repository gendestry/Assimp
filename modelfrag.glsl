#version 330 core

in vec4 norm;

out vec4 color;

vec3 lightPos = vec3(0.0,1.0,1.0);
vec3 lightColor = vec3(1.0, 1.0, 1.0);

void main() {
	vec3 normal = normalize(norm.xyz);
	vec3 lightDir = normalize(lightPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	color = vec4(diffuse,1.0);
}