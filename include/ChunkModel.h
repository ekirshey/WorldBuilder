#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

namespace chunk {
	struct Model {
		// Geometry info
		std::vector<GLfloat> vertices;
		std::vector<GLuint> indices;

		// Opengl buffers
		GLuint VAO;
		GLuint VBO;
		GLuint EBO;
	};

	void buildChunkModel(Model* m, glm::vec3 position, GLfloat width, int rows, int cols);
	void freeChunkModel(Model* m);
	void reloadVertexData(Model* m);
	void draw(Model* m);
}