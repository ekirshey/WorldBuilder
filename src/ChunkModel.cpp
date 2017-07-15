#include "ChunkModel.h"

namespace chunk {
	Model::Model(glm::vec3 localcoords, float width, int rows, int cols) {
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
		_bindOpenGLBuffers();
	}

	Model::~Model() {
		glDeleteVertexArrays(1, &_VAO);
		glDeleteBuffers(1, &_VBO);
		glDeleteBuffers(1, &_EBO);
	}

	void Model::_bindOpenGLBuffers() {
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

		glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
	}

	bool Model::vertexIntersectsWithRay(const Ray& ray, float ray_mag, unsigned int& vertex_index) const {
		auto ray_pos = ray.atPoint(ray_mag);
		for (int i = 0; i < _vertices.size(); i += 5) {
			glm::vec3 currentpoint = glm::vec3(_vertices[i], _vertices[i + 1], _vertices[i + 2]);
			if (ray.intersectWithPoint(ray_pos, currentpoint)) {
				vertex_index = i;
				return true;
			}
		}
		return false;
	}

	bool Model::faceIntersectsWithRay(const Ray & ray, unsigned int& face_index) const {
		bool ret = false;
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
		// Modify the vertex data
		glBindBuffer(GL_ARRAY_BUFFER, _VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * _vertices.size(), _vertices.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Model::addIndicesToVector(std::vector<unsigned int>& v) const {
		v.insert(v.end(), _indices.begin(), _indices.end());
	}

	void Model::draw() {
		glBindVertexArray(_VAO);
		glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}