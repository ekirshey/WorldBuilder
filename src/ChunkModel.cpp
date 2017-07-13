#include "ChunkModel.h"

namespace chunk {
	void buildChunkModel(Model* m, glm::vec3 position, GLfloat width, int rows, int cols) {
		GLfloat colwidth = width / cols;
		GLfloat rowwidth = width / rows;

		for (int i = 0; i <= rows; i++) {
			for (int j = 0; j <= cols; j++) {
				m->vertices.push_back(position.x + (j*colwidth));
				m->vertices.push_back(0.0f);;
				m->vertices.push_back(position.z - (i*rowwidth));

				m->vertices.push_back(position.x + (j*colwidth));
				m->vertices.push_back(position.z - (i*rowwidth));
			}
		}

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				// Left triangle
				m->indices.push_back((i*(cols + 1)) + j);
				m->indices.push_back(((i + 1)*(cols + 1)) + j);
				m->indices.push_back(((i + 1)*(cols + 1)) + j + 1);

				// Right triangle
				m->indices.push_back((i*(cols + 1)) + j);
				m->indices.push_back((i*(cols + 1)) + j + 1);
				m->indices.push_back(((i + 1)*(cols + 1)) + j + 1);
			}
		}

		glGenVertexArrays(1, &m->VAO);
		glGenBuffers(1, &m->VBO);
		glGenBuffers(1, &m->EBO);
		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(m->VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m->vertices.size(), m->vertices.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m->indices.size(), m->indices.data(), GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
	}

	void freeChunkModel(Model* m) {
		glDeleteVertexArrays(1, &m->VAO);
		glDeleteBuffers(1, &m->VBO);
		glDeleteBuffers(1, &m->EBO);
	}

	void reloadVertexData(Model* m) {
		// Modify the vertex data
		glBindBuffer(GL_ARRAY_BUFFER, m->VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * m->vertices.size(), m->vertices.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void draw(Model* m)
	{
		glBindVertexArray(m->VAO);
		glDrawElements(GL_TRIANGLES, m->indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}