#pragma once
#include <vector>
#include <GL/glew.h>
#include "Ray.h"
#include "ChunkModel.h"
#include "ShapePrimitives.h"

namespace chunk {
	class Geometry {
	public:
		Geometry(glm::vec3 localcoords, glm::vec3 translation, GLfloat width);
		~Geometry();

		bool intersectsWithRay(const Ray& ray, float& intersect_point) const;
		bool intersectsWithRay(const Ray& ray) const;
		bool intersectsWithCube(GLfloat leftbound,
								GLfloat rightbound,
								GLfloat frontbound,
								GLfloat backbound,
								bool& surrounds) const;
		bool intersectsWithCircle(const shapes::Circle& circle);

		void buildModelMatrix(glm::mat4& model) const;

		void vectorToChunkLocalCoords(glm::vec4& vec);

		glm::vec3 currentPosition() const {
			glm::mat4 transform;
			buildModelMatrix(transform);
			return transform * glm::vec4(_localcoords,1.0f);
		}
	private:
		// Geometry
		glm::vec3 _normal;
		glm::vec3 _offset;		// offset from the world origin (0,0,0)
		glm::vec3 _localcoords;
		glm::vec3 _translation;
		glm::vec3 _position;

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
