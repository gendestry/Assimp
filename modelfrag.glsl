#version 330 core
out vec4 color;

in vec2 texcoord;
in vec3 fragPos;
in vec3 norm;

uniform int numMaterials;
uniform vec3 viewPos;

struct Material {
	vec3 diffuseColor;
	vec3 specularColor;
	sampler2D diffuseTex;
	sampler2D specularTex;
};

uniform Material materials[10];

vec3 lightPos = vec3(1000.0,1000.0,1000.0);

void main() {
	// diffuse
	vec3 ambient = vec3(1.0) * 0.1;

	vec3 normal = normalize(norm);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diffk = max(dot(normal, lightDir), 0.1);
	vec4 diffuse = vec4(vec3(diffk), 1.0);

	// specular
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float speck = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
	vec4 specular = vec4(vec3(0.5 * speck), 1.0);

	vec4 texcolor = vec4(1.0);
	for(int i = 0; i < numMaterials; i++){
		diffuse *= vec4(materials[i].diffuseColor, 1.0);
		//specular *= vec4(materials[i].specularColor, 1.0);
		//diffuse *= texture(materials[i].diffuseTex, texcoord);
		//texcolor *= texture(materials[i].specularTex, texcoord);
	}
	
	//color = vec4(ambient + diffuse, 1.0) * texcolor + vec4(specular, 0.0);
	color = diffuse;
}