#pragma once
#include <vector>
#include <string>

#include <GL/glew.h>

#include "Shader.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
};

class Mesh {
private:
	std::vector<Vertex> vertexes;
	std::vector<unsigned int> indices;

	unsigned vao, vbo, ebo;
public:
	Mesh(std::vector<Vertex> vert, std::vector<unsigned int> ind);
	~Mesh();
	void render(const Shader& shader);
};

Mesh::Mesh(std::vector<Vertex> vert, std::vector<unsigned int> ind) : vertexes(vert), indices(ind) {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexes.size(), &vertexes[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));

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

void Mesh::render(const Shader& shader) {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
}