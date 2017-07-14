#include <iostream>
#include <glm/glm.hpp>
#include "ChunkGeometry.h"
#include <glm/ext.hpp>

namespace chunk {
	Geometry::Geometry(glm::vec3 position, GLfloat width)
		: _normal(0.0f, 1.0f, 0.0f) //hard code for now
		, _position(position)
		, _xmin(_position.x)
		, _xmax(_position.x + width)
		, _zmax(_position.z)
		, _zmin(_position.z - width)
	{
	}

	Geometry::~Geometry()
	{
	}

	bool Geometry::intersectsWithRay(const Ray & ray, float& intersect_point)
	{
		bool ret = false;
		if (ray.intersectWithPlane(_normal, _offset, intersect_point)) {
			auto ray_pos = ray.atPoint(intersect_point);

			if (ray_pos.x <= _xmax &&
				ray_pos.x >= _xmin &&
				ray_pos.z <= _zmax &&
				ray_pos.z >= _zmin
				)
			{
				ret = true;
			}
		}
		return ret;
	}

	bool Geometry::intersectsWithRay(const Ray & ray)
	{
		float dummy;
		return intersectsWithRay(ray, dummy);
	}

	bool Geometry::intersectsWithCube(GLfloat leftbound,
									  GLfloat rightbound,
									  GLfloat topbound,
									  GLfloat bottombound,
									  GLfloat frontbound,
									  GLfloat backbound) 
	{
		return true;
	}

	void Geometry::buildModelMatrix(glm::mat4 & model)
	{
		model = glm::translate(_position);
	}

}