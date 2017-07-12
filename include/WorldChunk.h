#pragma once
#include <vector>
#include <GL/glew.h>
#include "Ray.h"

class WorldChunk {
	public:
		WorldChunk(GLfloat x, GLfloat z, GLfloat width, int rows, int cols);
		~WorldChunk();

		bool vertexIntersectsWithRay(const Ray& ray, int& vertex_index);
		bool faceIntersectsWithRay(const Ray& ray, int& face_index);

		std::vector<GLuint> indicesInCube(GLfloat leftbound,
										  GLfloat rightbound,
										  GLfloat topbound,
										  GLfloat bottombound,
										  GLfloat frontbound,
										  GLfloat backbound);

		// probably handle this better
		void modifyVertex(int vertex, const glm::vec3& change);
		void modifyFace(int face, const glm::vec3& change);

		void draw();
	private:
		void _buildPlaneGeometry(GLfloat x, GLfloat z, GLfloat width, int rows, int cols);
		void _bindOpenGLBuffers();

		// Geometry
		std::vector<GLfloat> _vertices;
		std::vector<GLuint> _indices;
		glm::vec3 _normal;
		glm::vec3 _offset;		// offset from the world origin (0,0,0)
		GLfloat _xmax;
		GLfloat _xmin;
		GLfloat _zmax;
		GLfloat _zmin;

		// Opengl buffers
		GLuint _VAO;
		GLuint _VBO;
		GLuint _EBO;
};
