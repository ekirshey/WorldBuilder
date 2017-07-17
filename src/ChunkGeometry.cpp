#include <iostream>
#include <glm/glm.hpp>
#include "ChunkGeometry.h"
#include <glm/ext.hpp>

namespace chunk {
	Geometry::Geometry(glm::vec3 localcoords, glm::vec3 worldtransform, GLfloat width)
		: _normal(0.0f, 1.0f, 0.0f) //hard code for now
		, _localcoords(localcoords)
		, _worldtransform(worldtransform)
		, _xmax(_localcoords.x + _worldtransform.x + width)
		, _xmin(_localcoords.x + _worldtransform.x)
		, _zmax(_localcoords.z + _worldtransform.z + width)
		, _zmin(_localcoords.z + _worldtransform.z)
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

	void Geometry::buildModelMatrix(glm::mat4 & model) const
	{
		model = glm::translate(model, _worldtransform);
	}

	void Geometry::vectorToChunkLocalCoords(glm::vec4& vec)
	{
		glm::mat4 modelMat;
		buildModelMatrix(modelMat);
		vec = glm::inverse(modelMat) * vec;
	}

}