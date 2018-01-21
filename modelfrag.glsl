#version 330 core

in vec3 norm;

uniform vec3 objectColor = vec3(0.2,1.0,0.5);

vec3 lightPos = vec3(1.0,1.0,1.0);
vec3 lightColor = vec3(1.0);

void main() {
	vec3 normal = normalize(norm);
	vec3 lightDir = normalize(lightPos);
	float diff = max(dot(normal, lightDir), 0.2);
	vec3 diffuse = diff * lightColor;

	vec3 result = diffuse * objectColor;
	gl_FragColor = vec4(result,1.0);
}