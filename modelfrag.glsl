#version 330 core
out vec4 color;

in vec2 texcoord;
in vec3 fragPos;
in vec3 norm;

uniform vec3 viewPos;

uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform sampler2D diffuseTex;
uniform sampler2D specularTex;

vec3 lightPos = vec3(1000.0,1000.0,1000.0);

void main() {
	// diffuse
	vec3 normal = normalize(norm);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diffk = max(dot(normal, lightDir), 0.1);
	vec4 diffuse = vec4(vec3(diffk), 1.0) * vec4(diffuseColor, 1.0) * texture(diffuseTex, texcoord);

	// specular
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float speck = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
	vec4 specular = vec4(vec3(0.5 * speck), 1.0) * vec4(specularColor, 1.0) * texture(specularTex, texcoord);
	
	color = diffuse + specular;
}