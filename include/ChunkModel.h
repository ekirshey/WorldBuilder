#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Ray.h"

namespace chunk {
	class Model {
	public:
		Model(glm::vec3 position, GLfloat width, int rows, int cols);
		Model(const Model& m);
		~Model();

		bool vertexIntersectsWithRay(const Ray& ray, float ray_mag, unsigned int& vertex_index);
		bool faceIntersectsWithRay(const Ray& ray, unsigned int& face_index);
		std::vector<GLuint> indicesInCube(GLfloat leftbound,
										  GLfloat rightbound,
										  GLfloat topbound,
										  GLfloat bottombound,
										  GLfloat frontbound,
										  GLfloat backbound);

		void modifyVertex( int vertex, const glm::vec3& change);
		void modifyFace( int face, const glm::vec3& change);
		void reloadVertexData();

		void draw();
	private:
		void _bindOpenGLBuffers();
		// Geometry info
		std::vector<GLfloat> _vertices;
		std::vector<GLuint> _indices;

		// Opengl buffers
		GLuint _VAO;
		GLuint _VBO;
		GLuint _EBO;
	};

}