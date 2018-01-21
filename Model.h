#pragma once
#include <vector>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include <GL/glew.h>

#include <iostream>

struct Vertex {
	float xpos, ypos, zpos;
	float xnorm, ynorm, znorm;
};

struct Mesh {
	std::vector<Vertex> vertexes;
	std::vector<unsigned int> indices;

	unsigned vao, vbo, ebo;

	Mesh(std::vector<Vertex> vert, std::vector<unsigned int> ind) : vertexes(vert), indices(ind) {
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

	~Mesh() {
		glDeleteBuffers(1, &ebo);
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}

	void render() {
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
		glBindVertexArray(0);
	}
};

struct Model {
public:
	std::vector<Mesh*> meshes;
	Model(const char* filename) { loadModel(filename); }
	void render();
private:
	void loadModel(const char* filename);
	void processNodeRecursive(aiNode* node, const aiScene* scene);
	Mesh* createMesh(aiMesh* mesh);
};

void Model::render() {
	for (unsigned i = 0; i < meshes.size(); i++) {
		meshes[i]->render();
	}
}

void Model::loadModel(const char* filepath) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
	processNodeRecursive(scene->mRootNode, scene);
}

void Model::processNodeRecursive(aiNode* node, const aiScene* scene){
	for (int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(createMesh(mesh));
	}

	for (int i = 0; i < node->mNumChildren; ++i)
		processNodeRecursive(node->mChildren[i], scene);
}

Mesh* Model::createMesh(aiMesh* mesh){
	std::vector<Vertex> vertexes;
	std::vector<unsigned int> indices;

	for (int i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;
		if (mesh->HasPositions()) {
			vertex.xpos = mesh->mVertices[i].x;
			vertex.ypos = mesh->mVertices[i].y;
			vertex.zpos = mesh->mVertices[i].z;
		}
		if (mesh->HasNormals()) {
			vertex.xnorm = mesh->mNormals[i].x;
			vertex.ynorm = mesh->mNormals[i].y;
			vertex.znorm = mesh->mNormals[i].z;
		}
		vertexes.push_back(vertex);
	}

	for (int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}

	return new Mesh(vertexes, indices);
}
