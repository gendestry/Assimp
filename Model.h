#pragma once

#include <vector>
#include <iostream>
#include <string>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include <GL/glew.h>

#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"

class Model {
private:
	std::vector<Mesh*> meshes;
	std::vector<Material> materials;
	std::string directory;

	void processNodeRecursive(aiNode* node, const aiScene* scene);
	Mesh* createMesh(aiMesh* mesh, const aiScene* scene);
public:
	Model() {}
	Model(std::string filename) { loadModel(filename); }
	~Model();
	void render(const Shader& shader);
	void loadModel(std::string filepath);
};

Model::~Model() {
	for (int i = 0; i < materials.size(); i++) {
		glDeleteTextures(1, &materials[i].diffuseTex.id);
		glDeleteTextures(1, &materials[i].specularTex.id);
	}
}

void Model::render(const Shader& shader) {
	shader.use();
	for (unsigned i = 0; i < meshes.size(); i++) 
		meshes[i]->render(shader);
}

void Model::loadModel(std::string filepath) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_RemoveRedundantMaterials);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		std::cout << "Couldn't load the model\n";
	else {
		directory = filepath.substr(0, filepath.find_last_of('/'));

		// load materials
		materials.resize(scene->mNumMaterials);
		for (unsigned i = 0; i < scene->mNumMaterials; i++) { // TODO: normal, height
			const aiMaterial* mat = scene->mMaterials[i];
			aiColor3D color;

			// DIFFUSE COLOR
			if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) != AI_FAILURE)
				materials[i].diffuseColor = { color.r, color.g, color.b };
			else {
				materials[i].diffuseColor = { 1.0, 1.0, 1.0 };
				std::cout << "NO DIFFUSE COLOR WTF\n";
			}

			// SPECULAR COLOR
			if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) != AI_FAILURE)
				materials[i].specularColor = { color.r, color.g, color.b };
			else {
				materials[i].specularColor = { 1.0, 1.0, 1.0 };
				std::cout << "NO SPECULAR COLOR WTF\n";
			}

			// DIFFUSE TEXTURE
			if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
				aiString path;
				mat->GetTexture(aiTextureType_DIFFUSE, 0, &path, 0, 0, 0, 0, 0);
				materials[i].diffuseTex = Texture(directory + "/" + path.data, DIFFUSE);

				if (materials[i].diffuseTex.Success() == false)
					materials[i].diffuseTex = Texture("Resources/white.jpg", DIFFUSE);
			}
			else {
				materials[i].diffuseTex = Texture("Resources/white.jpg", DIFFUSE);
			}

			// SPECULAR TEXTURE
			if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0) {
				aiString path;
				mat->GetTexture(aiTextureType_SPECULAR, 0, &path, 0, 0, 0, 0, 0);
				materials[i].specularTex = Texture(directory + "/" + path.data, SPECULAR);

				if (materials[i].specularTex.Success() == false)
					materials[i].specularTex = Texture("Resources/white.jpg", SPECULAR);
			}
			else {
				materials[i].specularTex = Texture("Resources/white.jpg", SPECULAR);
			}
		}

		processNodeRecursive(scene->mRootNode, scene);
	}
}

void Model::processNodeRecursive(aiNode* node, const aiScene* scene){
	for (int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(createMesh(mesh, scene));
	}

	for (int i = 0; i < node->mNumChildren; ++i)
		processNodeRecursive(node->mChildren[i], scene);
}

Mesh* Model::createMesh(aiMesh* mesh, const aiScene* scene){
	std::vector<Vertex> vertexes;
	std::vector<unsigned int> indices;

	vertexes.resize(mesh->mNumVertices);
	indices.resize(mesh->mNumFaces * 3);

	for (int i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;

		vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		vertex.normal = mesh->HasNormals() ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z) : glm::vec3(0, 0, 0);
		vertex.texcoord = mesh->HasTextureCoords(0) ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0, 0);

		vertexes[i] = vertex;
	}

	for (int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; ++j)
			indices[i * 3 + j] = face.mIndices[j];
	}

	return new Mesh(vertexes, indices, materials[mesh->mMaterialIndex]);
}