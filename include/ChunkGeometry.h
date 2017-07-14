#pragma once
#include <vector>
#include <GL/glew.h>
#include "Ray.h"
#include "ChunkModel.h"

namespace chunk {
	class Geometry {
	public:
		Geometry(glm::vec3 position, GLfloat width);
		~Geometry();

		bool intersectsWithRay(const Ray& ray, float& intersect_point);
		bool intersectsWithRay(const Ray& ray);
		bool intersectsWithCube(GLfloat leftbound,
								GLfloat rightbound,
								GLfloat topbound,
								GLfloat bottombound,
								GLfloat frontbound,
								GLfloat backbound);

		void buildModelMatrix(glm::mat4& model);

	private:
		// Geometry
		glm::vec3 _normal;
		glm::vec3 _offset;		// offset from the world origin (0,0,0)
		glm::vec3 _position;

		GLfloat _yaw;
		GLfloat _roll;
		GLfloat _pitch;
		GLfloat _width;

		GLfloat _xmax;
		GLfloat _xmin;
		GLfloat _zmax;
		GLfloat _zmin;

	};
}
