#pragma once
#include <vector>
#include <string>

#include <GL/glew.h>

#include "Shader.h"
#include "Texture.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoord;
};

struct Material {
	Material(){}
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	Texture diffuseTex;
	Texture specularTex;
};

class Mesh {
private:
	std::vector<Vertex> vertexes;
	std::vector<unsigned> indices;
	std::vector<Material> materials;

	unsigned vao, vbo, ebo;
public:
	Mesh(std::vector<Vertex> vert, std::vector<unsigned int> ind, std::vector<Material> mats);
	~Mesh();
	void render(const Shader& shader);
};

Mesh::Mesh(std::vector<Vertex> vert, std::vector<unsigned int> ind, std::vector<Material> mats) : vertexes(vert), indices(ind), materials(mats) {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), &vertexes[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * indices.size(), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

Mesh::~Mesh() {
	for (int i = 0; i < materials.size(); i++)
		glDeleteTextures(1, &materials[i].diffuseTex.id);

	materials.clear();

	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Mesh::render(const Shader& shader) {
	shader.use();

	for (int i = 0; i < materials.size(); i++) {
		shader.setVec3(("materials[" + std::to_string(i) + "].diffuseColor").c_str(), materials[i].diffuseColor);
		shader.setVec3(("materials[" + std::to_string(i) + "].specularColor").c_str(), materials[i].specularColor);
		shader.setInt(("materials[" + std::to_string(i) + "].diffuseTex").c_str(), 0 + i * 2);
		materials[1].diffuseTex.bind(0 + i * 2);
		shader.setInt(("materials[" + std::to_string(i) + "].specularTex").c_str(), 1 + i * 2);
		materials[1].specularTex.bind(1 + i * 2);
	}

	shader.setInt("numMaterials", materials.size());
	
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
}