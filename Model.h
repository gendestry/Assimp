#pragma once
#define STB_IMAGE_IMPLEMENTATION

#include <vector>
#include <iostream>
#include <string>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include "stb_image.h"
#include "Shader.h"
#include "Mesh.h"

class Model {
public:
	Model(const char* filename) { loadModel(filename); }
	void render(const Shader& shader);
private:
	std::vector<Mesh*> meshes;
	std::string directory;

	void loadModel(std::string filename);
	void processNodeRecursive(aiNode* node, const aiScene* scene);
	Mesh* createMesh(aiMesh* mesh, const aiScene* scene);
};

void Model::render(const Shader& shader) {
	for (unsigned i = 0; i < meshes.size(); i++)
		meshes[i]->render(shader);
}

void Model::loadModel(std::string filepath) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		std::cout << "Couldnt load the damn model.....you done goofed" << std::endl;
	else {
		directory = filepath.substr(0, filepath.find_last_of('/'));
		processNodeRecursive(scene->mRootNode, scene);
	}
		
}

void Model::processNodeRecursive(aiNode* node, const aiScene* scene){
	for (int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(createMesh(mesh, scene));
	}

	for (int i = 0; i < node->mNumChildren; ++i)
		processNodeRecursive(node->mChildren[i], scene);
}

Mesh* Model::createMesh(aiMesh* mesh, const aiScene* scene){
	std::vector<Vertex> vertexes;
	std::vector<unsigned int> indices;

	for (int i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;
		if (mesh->HasPositions()) {
			vertex.position.x = mesh->mVertices[i].x;
			vertex.position.y = mesh->mVertices[i].y;
			vertex.position.z = mesh->mVertices[i].z;
		}
		if (mesh->HasNormals()) {
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
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