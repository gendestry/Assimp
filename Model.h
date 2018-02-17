#pragma once

#include <vector>
#include <iostream>
#include <string>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"

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
		std::cout << "Couldn't load the model\n";
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
	std::vector<Texture> textures;

	for (int i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;

		vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		vertex.normal = mesh->HasNormals() ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) : glm::vec3(0, 0, 0);
		vertex.texcoord = mesh->HasTextureCoords(0) ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0, 0);

		vertexes.push_back(vertex);
	}

	for (int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
	}

	for (unsigned i = 0; i < scene->mNumMaterials; i++) {
		const aiMaterial* mat = scene->mMaterials[i];
		// TODO: add multiple texture type support
		
		if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString path;
			mat->GetTexture(aiTextureType_DIFFUSE, 0, &path, 0, 0, 0, 0, 0);
			textures.emplace_back(directory + "/" + path.data);
		}
		if (textures.size() != i + 1)
			textures.emplace_back("Resources/white.jpg");
	}

	return new Mesh(vertexes, indices, textures);
}