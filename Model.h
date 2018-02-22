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

	void loadModel(const std::string& filename);
	void processNodeRecursive(aiNode* node, const aiScene* scene);
	Mesh* createMesh(aiMesh* mesh, const aiScene* scene);
};

void Model::render(const Shader& shader) {
	for (unsigned i = 0; i < meshes.size(); i++) 
		meshes[i]->render(shader);
}

void Model::loadModel(const std::string& filepath) {
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
		meshes.push_back(createMesh(mesh, scene));
	}

	for (int i = 0; i < node->mNumChildren; ++i)
		processNodeRecursive(node->mChildren[i], scene);
}

Mesh* Model::createMesh(aiMesh* mesh, const aiScene* scene){
	std::vector<Vertex> vertexes;
	std::vector<unsigned int> indices;
	std::vector<Material> mats;
	mats.resize(scene->mNumMaterials);

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

	for (unsigned i = 0; i < scene->mNumMaterials; i++) { // TODO: normal, height
		const aiMaterial* mat = scene->mMaterials[i];
		aiColor3D color;

		// DIFFUSE COLOR
		if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) != AI_FAILURE)
			mats[i].diffuseColor = { color.r, color.g, color.b };
		else {
			mats[i].diffuseColor = { 1.0, 1.0, 1.0 };
			std::cout << "NO DIFFUSE COLOR WTF\n";
		}

		// SPECULAR COLOR
		if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) != AI_FAILURE)
			mats[i].specularColor = { color.r, color.g, color.b };
		else {
			mats[i].specularColor = { 1.0, 1.0, 1.0 };
			std::cout << "NO SPECULAR COLOR WTF\n";
		}

		// DIFFUSE TEXTURE
		if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString path;
			mat->GetTexture(aiTextureType_DIFFUSE, 0, &path, 0, 0, 0, 0, 0);
			mats[i].diffuseTex = Texture(directory + "/" + path.data, DIFFUSE);

			if(mats[i].diffuseTex.Success() == false)
				mats[i].diffuseTex = Texture("Resources/white.jpg", DIFFUSE);
		}
		else {
			mats[i].diffuseTex = Texture("Resources/white.jpg", DIFFUSE);
		}

		// SPECULAR TEXTURE
		if (mat->GetTextureCount(aiTextureType_SPECULAR) > 0) {
			aiString path;
			mat->GetTexture(aiTextureType_SPECULAR, 0, &path, 0, 0, 0, 0, 0);
			mats[i].specularTex = Texture(directory + "/" + path.data, SPECULAR);

			if (mats[i].specularTex.Success() == false)
				mats[i].specularTex = Texture("Resources/white.jpg", SPECULAR);
		}
		else {
			mats[i].specularTex = Texture("Resources/white.jpg", SPECULAR);
		}

		/*if (mat->Get(AI_MATKEY_SHININESS, color) != AI_FAILURE)
			std::cout << color.r << " " << color.g << " " << color.b << "\n";*/

	}
	return new Mesh(vertexes, indices, mats);
}