#include <iostream>
#include <glm/glm.hpp>
#include "WorldChunk.h"
#include <glm/ext.hpp>


WorldChunk::WorldChunk(chunk::Model* m, glm::vec3 position, GLfloat width)
	: _model(m)
	, _normal(0.0f, 1.0f, 0.0f) //hard code for now
	, _position(position)
	, _xmin(_position.x)
	, _xmax(_position.x + width)
	, _zmax(_position.z)
	, _zmin(_position.z - width)
{
}

WorldChunk::~WorldChunk()
{
}

// I think it should have the point and not the index as the value
bool WorldChunk::vertexIntersectsWithRay(const Ray& ray, int& vertex_index) {
	float intersect_point = 0;
	if (ray.intersectWithPlane(_normal, _offset, intersect_point)) {
		auto ray_pos = ray.atPoint(intersect_point);

		if (ray_pos.x > _xmax ||
			ray_pos.x < _xmin ||
			ray_pos.z > _zmax ||
			ray_pos.z < _zmin
			) 
		{
			return false;
		}

		for (int i = 0; i < _model->vertices.size(); i += 5) {
			glm::vec3 currentpoint = glm::vec3(_model->vertices[i], _model->vertices[i + 1], _model->vertices[i + 2]);
			if (ray.intersectWithPoint(ray_pos, currentpoint)) {
				vertex_index = i;
				return true;
			}
		}
	}
	return false;
}

bool WorldChunk::faceIntersectsWithRay(const Ray& ray, int& face_index)
{
	bool ret = false;
	float intersect_point = 0;

	// Check if it even intersects with the plane
	if (ray.intersectWithPlane(_normal, _offset, intersect_point)) {
		auto ray_pos = ray.atPoint(intersect_point);

		if (ray_pos.x > _xmax ||
			ray_pos.x < _xmin ||
			ray_pos.z > _zmax ||
			ray_pos.z < _zmin
			)
		{
			return false;
		}
	}

	for (int i = 0; i < _model->indices.size(); i += 6) {
		glm::vec3 lt_0 = glm::vec3(_model->vertices[_model->indices[i] * 5], _model->vertices[_model->indices[i] * 5 + 1], _model->vertices[_model->indices[i] * 5 + 2]);
		glm::vec3 lt_1 = glm::vec3(_model->vertices[_model->indices[i + 1] * 5], _model->vertices[_model->indices[i + 1] * 5 + 1], _model->vertices[_model->indices[i + 1] * 5 + 2]);
		glm::vec3 lt_2 = glm::vec3(_model->vertices[_model->indices[i + 2] * 5], _model->vertices[_model->indices[i + 2] * 5 + 1], _model->vertices[_model->indices[i + 2] * 5 + 2]);
		if (ray.intersectWithTriangle(lt_0, lt_1, lt_2)) {
			ret = true;
			face_index = i;
			break;
		}

		glm::vec3 rt_0 = glm::vec3(_model->vertices[_model->indices[i + 3] * 5], _model->vertices[_model->indices[i + 3] * 5 + 1], _model->vertices[_model->indices[i + 3] * 5 + 2]);
		glm::vec3 rt_1 = glm::vec3(_model->vertices[_model->indices[i + 4] * 5], _model->vertices[_model->indices[i + 4] * 5 + 1], _model->vertices[_model->indices[i + 4] * 5 + 2]);
		glm::vec3 rt_2 = glm::vec3(_model->vertices[_model->indices[i + 5] * 5], _model->vertices[_model->indices[i + 5] * 5 + 1], _model->vertices[_model->indices[i + 5] * 5 + 2]);
		if (ray.intersectWithTriangle(rt_0, rt_1, rt_2)) {
			ret = true;
			face_index = i + 3;
			break;
		}
	}
	return ret;
}

std::vector<GLuint> WorldChunk::indicesInCube(GLfloat leftbound,
											  GLfloat rightbound,
											  GLfloat topbound,
											  GLfloat bottombound,
											  GLfloat frontbound,
											  GLfloat backbound)
{
	std::vector<GLuint> _indices_in_box;
	for (int i = 0; i < _model->vertices.size(); i += 5) {
		glm::vec3 currentpoint = glm::vec3(_model->vertices[i], _model->vertices[i + 1], _model->vertices[i + 2]);
		if ((currentpoint.x <= rightbound) && (currentpoint.x >= leftbound) &&
			//(currentpoint.y <= topbound) && (currentpoint.y >= bottombound) &&
			(currentpoint.z <= frontbound) && (currentpoint.z >= backbound) 
			) 
		{
			_indices_in_box.push_back(i);
		}
	}

	return _indices_in_box;
}

void WorldChunk::modifyVertex(int vertex, const glm::vec3& change)
{
	_model->vertices[vertex] += change.x;
	_model->vertices[vertex + 1] += change.y;
	_model->vertices[vertex + 2] += change.z;

}

void WorldChunk::modifyFace(int face, const glm::vec3 & change)
{
	_model->vertices[_model->indices[face] * 5] += change.x;
	_model->vertices[_model->indices[face] * 5 + 1] += change.y;
	_model->vertices[_model->indices[face] * 5 + 2] += change.z;

	_model->vertices[_model->indices[face + 1] * 5] += change.x;
	_model->vertices[_model->indices[face + 1] * 5 + 1] += change.y;
	_model->vertices[_model->indices[face + 1] * 5 + 2] += change.z;

	_model->vertices[_model->indices[face + 2] * 5] += change.x;
	_model->vertices[_model->indices[face + 2] * 5 + 1] += change.y;
	_model->vertices[_model->indices[face + 2] * 5 + 2] += change.z;

}

void WorldChunk::reloadVertexData()
{
	chunk::reloadVertexData(_model);
}

void WorldChunk::draw()
{
	chunk::draw(_model);
}

