#pragma once
#include <string>
#include <iostream>
#include <GL/glew.h>
#include "stb_image.h"

// QUick Helper for loading textures
GLuint LoadTexture(std::string filename, GLint colorformat) {
	int width, height;
	int n1;
	unsigned char* image = stbi_load(filename.c_str(), &width, &height, &n1, 0);
	if (image == NULL)
		std::cout << "Failed to load image" << std::endl;

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Set our texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, colorformat, width, height, 0, colorformat, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texture;
}