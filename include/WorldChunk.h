#pragma once
#include <vector>
#include <GL/glew.h>
#include "Ray.h"
#include "ChunkModel.h"

class WorldChunk {
	public:
		WorldChunk(chunk::Model* m, glm::vec3 position, GLfloat width);
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
		void reloadVertexData();

		void draw();

	private:
		// Model info
		chunk::Model* _model;

		// Geometry
		glm::vec3 _normal;
		glm::vec3 _offset;		// offset from the world origin (0,0,0)
		glm::vec3 _position;
		GLfloat _xmax;
		GLfloat _xmin;
		GLfloat _zmax;
		GLfloat _zmin;


};
