#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Ray.h"

namespace chunk {
	class Model {
	public:
		Model(glm::vec3 localcoords, float width, int rows, int cols);
		Model(const Model& m);
		~Model();

		bool vertexIntersectsWithRay(const Ray& ray, float ray_mag, unsigned int& vertex_index) const;
		bool faceIntersectsWithRay(const Ray& ray, unsigned int& face_index) const;
		void addIndicesToVector(std::vector<unsigned int>& v) const;

		std::vector<GLuint> indicesInCube(GLfloat leftbound,
										  GLfloat rightbound,
										  GLfloat topbound,
										  GLfloat bottombound,
										  GLfloat frontbound,
										  GLfloat backbound);

		void indicesInCube( std::vector<unsigned int>& indices,
											   GLfloat leftbound,
											   GLfloat rightbound,
											   GLfloat topbound,
											   GLfloat bottombound,
											   GLfloat frontbound,
											   GLfloat backbound);

		void modifyVertex( unsigned int vertex, const glm::vec3& change);
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