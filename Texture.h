#pragma once
#include <gl/glew.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Texture {
private:
	unsigned texid;
	int width, height, nComp;
	bool success;
public:
	std::string path;

	Texture(std::string path);
	~Texture();

	void bind(unsigned pos = 0) const;
	void unbind() const;

	inline bool Success() const { return success; }
	inline unsigned ID() const { return texid; }
};

Texture::Texture(std::string path) : path(path) {
	stbi_set_flip_vertically_on_load(1);

	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nComp, 0);

	if (data) {
		int format = GL_RGBA;
		if (nComp == 3)
			format = GL_RGB;
		
		glGenTextures(1, &texid);
		glBindTexture(GL_TEXTURE_2D, texid);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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

Texture::~Texture() {
	glDeleteTextures(1, &texid);
}

void Texture::bind(unsigned pos) const {
	glActiveTexture(GL_TEXTURE0 + pos);
	glBindTexture(GL_TEXTURE_2D, texid);
}

void Texture::unbind() const {
	glBindTexture(GL_TEXTURE_2D, 0);
}