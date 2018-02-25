#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 position;

uniform vec3 viewPos;
uniform samplerCube skybox;

void main() {        
	vec3 I = normalize(position - viewPos);
	vec3 R;

	if(true) {
		R = reflect(I, normalize(normal));
	}
	else {
		float ratio = 1.00 / 1.52;
		R = refract(I, normalize(normal), ratio);
	}

	FragColor = vec4(texture(skybox, R).rgb, 1.0);
}