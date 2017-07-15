#pragma once
#include <vector>
#include <GL/glew.h>
#include "Ray.h"
#include "ChunkModel.h"

namespace chunk {
	class Geometry {
	public:
		Geometry(glm::vec3 localcoords, glm::vec3 worldtransform, GLfloat width);
		~Geometry();

		bool intersectsWithRay(const Ray& ray, float& intersect_point) const;
		bool intersectsWithRay(const Ray& ray) const;
		bool intersectsWithCube(GLfloat leftbound,
								GLfloat rightbound,
								GLfloat frontbound,
								GLfloat backbound,
								bool& surrounds) const;

		void buildModelMatrix(glm::mat4& model) const;

		void vectorToChunkLocalCoords(glm::vec4& vec);
	private:
		// Geometry
		glm::vec3 _normal;
		glm::vec3 _offset;		// offset from the world origin (0,0,0)
		glm::vec3 _localcoords;
		glm::vec3 _worldtransform;

		GLfloat _yaw;
		GLfloat _roll;
		GLfloat _pitch;
		GLfloat _width;

		//2d plane so no y vals
		GLfloat _xmax;
		GLfloat _xmin;
		GLfloat _zmax;
		GLfloat _zmin;

	};
}
