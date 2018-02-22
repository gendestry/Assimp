#pragma once
#include <gl/glew.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

enum Type {
	AMBIENT,
	DIFFUSE,
	SPECULAR,
	NORMAL,
	HEIGHT
};

class Texture {
private:
	int width, height, nComp;
	bool success;
public:
	std::string path;
	unsigned id;
	Type type;

	Texture() { id = 0; }
	Texture(std::string path, Type t);

	void bind(unsigned pos = 0) const;
	void unbind() const;

	inline bool Success() const { return success; }
};

Texture::Texture(std::string path, Type t) : path(path), type(t) {
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nComp, 0);

	if (data) {
		int format = GL_RGBA;
		if (nComp == 3)
			format = GL_RGB;
		
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		(nComp == 3) ? glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data) : glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		success = true;
	}
	else {
		std::cout << "Invalid path: " << path << "\n";
		success = false;
	}
	stbi_image_free(data);
}

void Texture::bind(unsigned pos) const {
	glActiveTexture(GL_TEXTURE0 + pos);
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind() const {
	glBindTexture(GL_TEXTURE_2D, 0);
}