#include "ChunkModel.h"

#include <iostream>

namespace chunk {
	Model::Model(glm::vec3 localcoords, float width, int rows, int cols) 
		: _width(width)
		, _rows(rows)
		, _cols(cols)
		, _indices(0)
		, _vertices(0)
		, _swapbuffers(false)
	{
		GLfloat colwidth = width / cols;
		GLfloat rowwidth = width / rows;
		
		for (int i = 0; i <= rows; i++) {
			for (int j = 0; j <= cols; j++) {
				_vertices.push_back(localcoords.x + (j*colwidth));
				_vertices.push_back(0.0f);;
				_vertices.push_back(localcoords.z + (i*rowwidth));

				_vertices.push_back(localcoords.x + (j*colwidth));
				_vertices.push_back(localcoords.z + (i*rowwidth));
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

	Model::Model(const Model & m)
	{
		_vertices = m._vertices;
		_indices = m._indices;
		_width = m._width;
		_rows = m._rows;
		_cols = m._cols;
		_bindOpenGLBuffers();
	}

	Model::~Model() {
		glDeleteVertexArrays(1, &_VAO_active);
		glDeleteBuffers(1, &_VBO_active);
		glDeleteBuffers(1, &_EBO_active);

		glDeleteVertexArrays(1, &_VAO_back);
		glDeleteBuffers(1, &_VBO_back);
		glDeleteBuffers(1, &_EBO_back);
	}

	// Helper function
	void Model::_loadVAOData(GLuint& vao, GLuint& vbo, GLuint& ebo) {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * _vertices.size(), _vertices.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * _indices.size(), _indices.data(), GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
	}

	void Model::_bindOpenGLBuffers() {
		_loadVAOData(_VAO_active, _VBO_active, _EBO_active);
		_loadVAOData(_VAO_back, _VBO_back, _EBO_back);
	}

	bool Model::vertexIntersectsWithPoint(glm::vec3 ray_pos, unsigned int& vertex_index) const {
		// Skip rows above the raypos
		int startIndex = (ray_pos.z / (_width / _rows));
		startIndex = startIndex * _cols * 5;
		for (int i = 0; i < _vertices.size(); i += 5) {
			glm::vec3 currentpoint = glm::vec3(_vertices[i], _vertices[i + 1], _vertices[i + 2]);
			if (RayUtils::intersectWithPoint(ray_pos, currentpoint)) {
				vertex_index = i;
				return true;
			}
		}
		return false;
	}

	bool Model::faceIntersectsWithPoint(const Ray & ray, unsigned int& face_index) const {
		bool ret = false;
		glm::vec3 lt_0;
		glm::vec3 lt_1;
		glm::vec3 lt_2;

		glm::vec3 rt_0;
		glm::vec3 rt_1;
		glm::vec3 rt_2;
		for (int i = 0; i < _indices.size(); i += 6) {
			lt_0 = glm::vec3(_vertices[_indices[i] * 5], _vertices[_indices[i] * 5 + 1], _vertices[_indices[i] * 5 + 2]);
			lt_1 = glm::vec3(_vertices[_indices[i + 1] * 5], _vertices[_indices[i + 1] * 5 + 1], _vertices[_indices[i + 1] * 5 + 2]);
			lt_2 = glm::vec3(_vertices[_indices[i + 2] * 5], _vertices[_indices[i + 2] * 5 + 1], _vertices[_indices[i + 2] * 5 + 2]);
			if (ray.intersectWithTriangle(lt_0, lt_1, lt_2)) {
				ret = true;
				face_index = i;
				break;
			}

			rt_0 = glm::vec3(_vertices[_indices[i + 3] * 5], _vertices[_indices[i + 3] * 5 + 1], _vertices[_indices[i + 3] * 5 + 2]);
			rt_1 = glm::vec3(_vertices[_indices[i + 4] * 5], _vertices[_indices[i + 4] * 5 + 1], _vertices[_indices[i + 4] * 5 + 2]);
			rt_2 = glm::vec3(_vertices[_indices[i + 5] * 5], _vertices[_indices[i + 5] * 5 + 1], _vertices[_indices[i + 5] * 5 + 2]);
			if (ray.intersectWithTriangle(rt_0, rt_1, rt_2)) {
				ret = true;
				face_index = i + 3;
				break;
			}
		}
		return ret;
	}

	bool Model::faceIntersectsWithPoint(glm::vec3 ray_pos, unsigned int& face_index) const
	{
		bool ret = false;
		glm::vec3 lt_0;
		glm::vec3 lt_1;
		glm::vec3 lt_2;

		glm::vec3 rt_0;
		glm::vec3 rt_1;
		glm::vec3 rt_2;
		// Skip rows above the raypos
		int startIndex = (ray_pos.z / (_width / _rows));
		startIndex = startIndex * _cols * 6;
		for (int i = startIndex; i < _indices.size(); i += 6) {
			lt_0 = glm::vec3(_vertices[_indices[i] * 5], _vertices[_indices[i] * 5 + 1], _vertices[_indices[i] * 5 + 2]);
			lt_1 = glm::vec3(_vertices[_indices[i + 1] * 5], _vertices[_indices[i + 1] * 5 + 1], _vertices[_indices[i + 1] * 5 + 2]);
			lt_2 = glm::vec3(_vertices[_indices[i + 2] * 5], _vertices[_indices[i + 2] * 5 + 1], _vertices[_indices[i + 2] * 5 + 2]);
			if (RayUtils::intersectWithTriangle(ray_pos,lt_0, lt_1, lt_2)) {
				ret = true;
				face_index = i;
				break;
			}

			rt_0 = glm::vec3(_vertices[_indices[i + 3] * 5], _vertices[_indices[i + 3] * 5 + 1], _vertices[_indices[i + 3] * 5 + 2]);
			rt_1 = glm::vec3(_vertices[_indices[i + 4] * 5], _vertices[_indices[i + 4] * 5 + 1], _vertices[_indices[i + 4] * 5 + 2]);
			rt_2 = glm::vec3(_vertices[_indices[i + 5] * 5], _vertices[_indices[i + 5] * 5 + 1], _vertices[_indices[i + 5] * 5 + 2]);
			if (RayUtils::intersectWithTriangle(ray_pos, rt_0, rt_1, rt_2)) {
				ret = true;
				face_index = i + 3;
				break;
			}
		}
		return ret;
	}

	std::vector<GLuint> Model::indicesInCube(GLfloat leftbound, GLfloat rightbound, GLfloat topbound, GLfloat bottombound, GLfloat frontbound, GLfloat backbound)
	{
		std::vector<GLuint> indices_in_box;
		indicesInCube(indices_in_box, leftbound, rightbound, topbound, bottombound, frontbound, backbound);
		return indices_in_box;
	}

	void Model::indicesInCube(std::vector<unsigned int>& indices,
												 GLfloat leftbound,
												 GLfloat rightbound,
												 GLfloat topbound,
												 GLfloat bottombound,
												 GLfloat frontbound,
												 GLfloat backbound) 
	{
		for (int i = 0; i < _vertices.size(); i += 5) {
			glm::vec3 currentpoint = glm::vec3(_vertices[i], _vertices[i + 1], _vertices[i + 2]);
			if ((currentpoint.x <= rightbound) && (currentpoint.x >= leftbound) &&
				//(currentpoint.y <= topbound) && (currentpoint.y >= bottombound) &&
				(currentpoint.z <= frontbound) && (currentpoint.z >= backbound)
				)
			{
				indices.push_back(i);
			}
		}
	}

	// vertices are an int referring to the start index of the xyz values
	void Model::verticesInCircle(const shapes::Circle& circle, std::vector<unsigned int>& vertices)
	{
		float r2 = circle._radius * circle._radius;
		int startIndex = ((circle._center.z - circle._radius) / (_width / _rows));
		startIndex = startIndex * _cols * 5;
		if (startIndex < 0) {
			startIndex = 0;
		}

		int endIndex = ((circle._center.z + circle._radius) / (_width / _rows));
		endIndex = endIndex * _cols * 5;
		if (endIndex > _vertices.size()) {
			endIndex = _vertices.size();
		}

		for (int i = startIndex; i < endIndex; i += 5) {
			auto x = _vertices[i];
			auto z = _vertices[i + 2];
			auto h = circle._center.x;
			auto k = circle._center.z;
			if ( (x-h)*(x-h) + (z-k)*(z-k) <= r2) {
				vertices.push_back(i);
			}
		}
	}

	void Model::modifyVertex( unsigned int vertex, const glm::vec3& change)
	{
		_vertices[vertex] += change.x;
		_vertices[vertex + 1] += change.y;
		_vertices[vertex + 2] += change.z;

	}

	void Model::modifyFace( int face, const glm::vec3 & change)
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

	void Model::reloadVertexData() {
		if (_swapbuffers) {
			return;
		}
		// Modify the vertex data
		glBindBuffer(GL_ARRAY_BUFFER, _VBO_back);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * _vertices.size(), NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * _vertices.size(), _vertices.data());
		_fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		_swapbuffers = true;
	}

	bool Model::vertexPosition(int vertexid, glm::vec3& vertexposition) const
	{
		if (vertexid >= 0 && vertexid < _vertices.size()) {
			vertexposition = glm::vec3(_vertices[vertexid], _vertices[vertexid+1], _vertices[vertexid + 2]);
			return true;
		}
		return false;

	}

	bool Model::facePositions(int index, std::vector<glm::vec3>& facepositions) const
	{
		if (index >= 0 && index < _indices.size()) {
			facepositions.push_back(glm::vec3(_vertices[_indices[index] * 5],
											  _vertices[_indices[index] * 5 + 1],
											  _vertices[_indices[index] * 5 + 2]));
			facepositions.push_back(glm::vec3(_vertices[_indices[index + 1] * 5], 
											  _vertices[_indices[index + 1] * 5 + 1],
											  _vertices[_indices[index + 1] * 5 + 2]));
			facepositions.push_back(glm::vec3(_vertices[_indices[index + 2] * 5],
											  _vertices[_indices[index + 2] * 5 + 1],
											  _vertices[_indices[index + 2] * 5 + 2]));

			return true;
		}
		return false;
	}

	void Model::addIndicesToVector(std::vector<unsigned int>& v) const {
		v.insert(v.end(), _indices.begin(), _indices.end());
	}

	void Model::draw() {
		glBindVertexArray(_VAO_active);
		glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		if (_swapbuffers) {
			// glGetSynciv 
			auto ret = glClientWaitSync(_fence, 0, 0);
			if (ret != GL_TIMEOUT_EXPIRED) {
				GLuint tempvao = _VAO_active;
				GLuint tempvbo = _VBO_active;
				GLuint tempebo = _EBO_active;

				_VAO_active = _VAO_back;
				_VBO_active = _VBO_back;
				_EBO_active = _EBO_back;

				_VAO_back = tempvao;
				_VBO_back = tempvbo;
				_EBO_back = tempebo;
				_swapbuffers = false;
			}
		}
	}
}