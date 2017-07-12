#include <iostream>
#include <glm/glm.hpp>
#include "WorldChunk.h"
#include <glm/ext.hpp>


WorldChunk::WorldChunk(GLfloat x, GLfloat z, GLfloat width, int rows, int cols) 
	: _normal(0.0f, 1.0f, 0.0f) //hard code for now
{
	_buildPlaneGeometry(x, z, width, rows, cols);
	_bindOpenGLBuffers();
}

WorldChunk::~WorldChunk()
{
	glDeleteVertexArrays(1, &_VAO);
	glDeleteBuffers(1, &_VBO);
	glDeleteBuffers(1, &_EBO);
}

void WorldChunk::_buildPlaneGeometry(GLfloat x, GLfloat z, GLfloat width, int rows, int cols) {
	GLfloat colwidth = width / cols;
	GLfloat rowwidth = width / rows;

	_xmin = x;
	_xmax = x + width;
	_zmax = z;
	_zmin = z - width;

	for (int i = 0; i <= rows; i++) {
		for (int j = 0; j <= cols; j++) {
			_vertices.push_back(x + (j*colwidth));
			_vertices.push_back(0.0f);;
			_vertices.push_back(z - (i*rowwidth));

			_vertices.push_back(x + (j*colwidth));
			_vertices.push_back(z - (i*rowwidth));
		}
	}

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			// Left triangle
			_indices.push_back((i*(cols + 1)) + j);
			_indices.push_back(((i + 1)*(cols + 1)) + j);
			_indices.push_back(((i + 1)*(cols + 1)) + j + 1);

			// Right triangle
			_indices.push_back((i*(cols + 1)) + j);
			_indices.push_back((i*(cols + 1)) + j + 1);
			_indices.push_back(((i + 1)*(cols + 1)) + j + 1);
		}
	}
}

void WorldChunk::_bindOpenGLBuffers()
{
	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO);
	glGenBuffers(1, &_EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * _vertices.size(), _vertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * _indices.size(), _indices.data(), GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO

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

		for (int i = 0; i < _vertices.size(); i += 5) {
			glm::vec3 currentpoint = glm::vec3(_vertices[i], _vertices[i + 1], _vertices[i + 2]);
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

	for (int i = 0; i < _indices.size(); i += 6) {
		glm::vec3 lt_0 = glm::vec3(_vertices[_indices[i] * 5], _vertices[_indices[i] * 5 + 1], _vertices[_indices[i] * 5 + 2]);
		glm::vec3 lt_1 = glm::vec3(_vertices[_indices[i + 1] * 5], _vertices[_indices[i + 1] * 5 + 1], _vertices[_indices[i + 1] * 5 + 2]);
		glm::vec3 lt_2 = glm::vec3(_vertices[_indices[i + 2] * 5], _vertices[_indices[i + 2] * 5 + 1], _vertices[_indices[i + 2] * 5 + 2]);
		if (ray.intersectWithTriangle(lt_0, lt_1, lt_2)) {
			ret = true;
			face_index = i;
			break;
		}

		glm::vec3 rt_0 = glm::vec3(_vertices[_indices[i + 3] * 5], _vertices[_indices[i + 3] * 5 + 1], _vertices[_indices[i + 3] * 5 + 2]);
		glm::vec3 rt_1 = glm::vec3(_vertices[_indices[i + 4] * 5], _vertices[_indices[i + 4] * 5 + 1], _vertices[_indices[i + 4] * 5 + 2]);
		glm::vec3 rt_2 = glm::vec3(_vertices[_indices[i + 5] * 5], _vertices[_indices[i + 5] * 5 + 1], _vertices[_indices[i + 5] * 5 + 2]);
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
	for (int i = 0; i < _vertices.size(); i += 5) {
		glm::vec3 currentpoint = glm::vec3(_vertices[i], _vertices[i + 1], _vertices[i + 2]);
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
	_vertices[vertex] += change.x;
	_vertices[vertex + 1] += change.y;
	_vertices[vertex + 2] += change.z;


}

void WorldChunk::modifyFace(int face, const glm::vec3 & change)
{
	_vertices[_indices[face] * 5] += change.x;
	_vertices[_indices[face] * 5 + 1] += change.y;
	_vertices[_indices[face] * 5 + 2] += change.z;

	_vertices[_indices[face + 1] * 5] += change.x;
	_vertices[_indices[face + 1] * 5 + 1] += change.y;
	_vertices[_indices[face + 1] * 5 + 2] += change.z;

	_vertices[_indices[face + 2] * 5] += change.x;
	_vertices[_indices[face + 2] * 5 + 1] += change.y;
	_vertices[_indices[face + 2] * 5 + 2] += change.z;

}

void WorldChunk::reloadVertexData()
{
	// Modify the vertex data
	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * _vertices.size(), _vertices.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void WorldChunk::draw()
{
	glBindVertexArray(_VAO);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

