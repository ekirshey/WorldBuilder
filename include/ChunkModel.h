#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Ray.h"
#include "ShapePrimitives.h"

namespace chunk {
	class Model {
	public:
		Model(glm::vec3 localcoords, float width, int rows, int cols);
		Model(const Model& m);
		~Model();

		bool vertexIntersectsWithPoint(glm::vec3 ray_pos, unsigned int& vertex_index) const;
		bool faceIntersectsWithPoint(const Ray& ray, unsigned int& face_index) const;
		bool faceIntersectsWithPoint(glm::vec3 ray_pos, unsigned int& face_index) const;

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

		void verticesInCircle(const shapes::Circle& circle, std::vector<unsigned int>& vertices);

		void modifyVertex( unsigned int vertex, const glm::vec3& change);
		void modifyFace( int face, const glm::vec3& change);
		void reloadVertexData();

		bool vertexPosition(int vertexid, glm::vec3& vertexposition) const;
		bool facePositions(int index, std::vector<glm::vec3>& facepositions) const;

		void draw();
	private:
		void _bindOpenGLBuffers();
		// Geometry info
		float _width;
		int _rows;
		int _cols;
		std::vector<GLfloat> _vertices;
		std::vector<GLuint> _indices;

		// Opengl buffers
		GLuint _VAO;
		GLuint _VBO;
		GLuint _EBO;
	};

}