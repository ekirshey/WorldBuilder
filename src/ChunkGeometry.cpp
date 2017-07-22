#include <iostream>
#include <glm/glm.hpp>
#include "ChunkGeometry.h"
#include <glm/ext.hpp>

namespace chunk {
	Geometry::Geometry(glm::vec3 localcoords, glm::vec3 translation, GLfloat width)
		: _normal(0.0f, 1.0f, 0.0f) //hard code for now
		, _localcoords(localcoords)
		, _translation(translation)
		, _position(localcoords + translation)
		, _width(width)
		, _xmax(_localcoords.x + _translation.x + width)
		, _xmin(_localcoords.x + _translation.x)
		, _zmax(_localcoords.z + _translation.z + width)
		, _zmin(_localcoords.z + _translation.z)
	{

	}

	Geometry::~Geometry()
	{
	}

	bool Geometry::intersectsWithRay(const Ray & ray, float& intersect_point) const
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

	bool Geometry::intersectsWithRay(const Ray & ray) const
	{
		float dummy;
		return intersectsWithRay(ray, dummy);
	}

	bool Geometry::intersectsWithCube(GLfloat leftbound,
									  GLfloat rightbound,
									  GLfloat frontbound,
									  GLfloat backbound,
									  bool& surrounds) const
	{
		surrounds = false;
		if (_xmax <= rightbound && _xmin >= leftbound &&
			_zmax <= frontbound  && _zmin >= backbound)
		{
			surrounds = true;
			return true;
		}
		if ( ( ( rightbound <= _xmax && rightbound >= _xmin ) ||
			   ( leftbound  <= _xmax && leftbound  >= _xmin ) ) &&
			 ( ( frontbound <= _zmax && frontbound >= _zmin ) || 
			   ( backbound  <= _zmax && backbound  >= _zmin ) )
			)
		{
			return true;
		}

		return false;
	}

	bool Geometry::intersectsWithCircle(const shapes::Circle & circle)
	{
		// circle + radius or circle - radius within bounds
		auto diff = glm::abs(circle._center - glm::vec3(_position.x + _width / 2, 0.0f, _position.z + _width / 2));
		if (diff.x > (circle._radius + _width / 2)) {
			return false;
		}
		else if (diff.z > (circle._radius + _width / 2)) {
			return false;
		}

		// Do I need to do a corner check?

		return true;
	}

	void Geometry::buildModelMatrix(glm::mat4 & model) const
	{
		model = glm::translate(model, _translation);
	}

	void Geometry::vectorToChunkLocalCoords(glm::vec4& vec)
	{
		glm::mat4 modelMat;
		buildModelMatrix(modelMat);
		vec = glm::inverse(modelMat) * vec;
	}

}