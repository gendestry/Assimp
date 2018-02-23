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
	Material() {}
	glm::vec3 diffuseColor;
	glm::vec3 specularColor;
	Texture diffuseTex;
	Texture specularTex;
};

class Mesh {
private:
	std::vector<Vertex> vertexes;
	std::vector<unsigned> indices;
	const Material& material;

	unsigned vao, vbo, ebo;
public:
	Mesh(std::vector<Vertex> vert, std::vector<unsigned int> ind, const Material& mat);
	~Mesh();
	void render(Shader& shader);
};

Mesh::Mesh(std::vector<Vertex> vert, std::vector<unsigned int> ind, const Material& mat) : vertexes(vert), indices(ind), material(mat) {
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
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void Mesh::render(Shader& shader) {
	shader.setVec3("diffuseColor", material.diffuseColor);
	shader.setVec3("specularColor", material.specularColor);
	shader.setInt("diffuseTex", 0);
	material.diffuseTex.bind(0);
	shader.setInt("specularTex", 1);
	material.specularTex.bind(1);
	
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}