#pragma once
#include <vector>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include <GL/glew.h>

struct MyMesh {
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	unsigned vao, vbo, ebo;

	MyMesh(std::vector<float> vert, std::vector<unsigned int> ind) : vertices(vert), indices(ind) {
		float* v = &vertices[0];
		unsigned* i = &indices[0];

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), v, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * vertices.size(), i, GL_STATIC_DRAW);

		glBindVertexArray(0);
	}
};

struct MyModel {
	std::vector<MyMesh*> meshes;

	MyModel(const char* filename) { loadModel(filename); }

	void loadModel(const char* filename);
	void processNodeRecursive(aiNode* node, const aiScene* scene);
	MyMesh* createMesh(aiMesh* mesh, const aiScene* scene);
};

void MyModel::loadModel(const char* filepath) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);
	processNodeRecursive(scene->mRootNode, scene);
}

void MyModel::processNodeRecursive(aiNode* node, const aiScene* scene){
	for (int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(createMesh(mesh, scene));
	}
}

MyMesh* MyModel::createMesh(aiMesh* mesh, const aiScene* scene){
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	for (int i = 0; i < mesh->mNumVertices; ++i) {
		if (mesh->HasPositions()) {
			vertices.push_back(mesh->mVertices[i].x);
			vertices.push_back(mesh->mVertices[i].y);
			vertices.push_back(mesh->mVertices[i].z);
		}
	}

	for (int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}

	return new MyMesh(vertices, indices);
}
