#include "ShapePrimitives.h"
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

namespace shapes {
	namespace{
		// Cube values
		const std::vector<GLfloat> bordered_cube = {
			-0.5, -0.5, -0.5,
			0.5, -0.5, -0.5,
			0.5,  0.5, -0.5,
			-0.5,  0.5, -0.5,
			-0.5, -0.5,  0.5,
			0.5, -0.5,  0.5,
			0.5,  0.5,  0.5,
			-0.5,  0.5,  0.5,
		};
		const std::vector<unsigned int> bordered_cube_indices{
			0,1,1,2,2,3,3,0, // First square
			0,4,1,5,2,6,3,7, // connections
			4,5,5,6,6,7,7,4  // Second square
		};
		GLuint BORDERCUBEVBO, BORDERCUBEVAO, BORDERCUBEEBO;

		// Square
		const std::vector<GLfloat> square = {
			0.0,  0.0,  0.0,
			0.0, -1.0,  0.0,
			1.0, -1.0,  0.0,
			1.0,  0.0,  0.0,
		};
		const std::vector<unsigned int> square_indices{
			0,1,1,2,2,3,3,0, // First square
		};
		GLuint SQUAREVBO, SQUAREVAO, SQUAREEBO;

		// Triangle
		std::vector<GLfloat> triangle;
		std::vector<unsigned int> triangle_indices;
		GLuint TRIANGLECUBEVBO, TRIANGLECUBEVAO, TRIANGLECUBEEBO;

	}

	void _recurseTriangle(const glm::vec3& top, const glm::vec3& bl, const glm::vec3& br, std::vector<glm::vec3>& pts, std::vector<unsigned int>& indices, int recurse_steps) {
		if (recurse_steps == 0) {
			return;
		}

		glm::vec3 old_top = top;
		glm::vec3 old_bl = bl;
		glm::vec3 old_br = br;

		glm::vec3 new_top;
		glm::vec3 new_bl;
		glm::vec3 new_br;

		new_top = (old_br - old_bl)*0.5f + old_bl;
		new_bl = (old_top - bl)*0.5f + old_bl;
		new_br = (old_top - old_br)*0.5f + old_br;

		indices.push_back(pts.size());
		pts.push_back(new_top);
		indices.push_back(pts.size());
		pts.push_back(new_bl);
		indices.push_back(pts.size());
		pts.push_back(new_br);


		_recurseTriangle(old_top, new_bl, new_br, pts, indices, recurse_steps - 1);
		_recurseTriangle(new_bl, old_bl, new_top, pts, indices, recurse_steps - 1);
		_recurseTriangle(new_br, new_top, old_br, pts, indices, recurse_steps - 1);
		_recurseTriangle(new_top, new_bl, new_br, pts, indices, recurse_steps - 1);

	}

	void _initializeTriangle() {
		std::vector<glm::vec3> t_verts;
		triangle_indices.push_back(t_verts.size());
		t_verts.push_back(glm::vec3(1.0f, 1.0f, 1.0f));  // top point
		triangle_indices.push_back(t_verts.size());
		t_verts.push_back(glm::vec3(0.0f, 1.0f, 0.0f));  // bottom left
		triangle_indices.push_back(t_verts.size());
		t_verts.push_back(glm::vec3(2.0f, 1.0f, 0.0f));  // bottom right

		_recurseTriangle(t_verts[0], t_verts[1], t_verts[2], t_verts, triangle_indices,3);

		glm::vec3 center(0.5f, 0.5f, 0.5f);
		float length = 0.5f;
		for (const auto& v : t_verts) {
			// normalize
			auto dx = v.x - center.x;
			auto dy = v.y - center.y;
			auto dz = v.z - center.z;

			dx = dx * length / glm::distance(center, v);
			dy = dy * length / glm::distance(center, v);
			dz = dz * length / glm::distance(center, v);
			glm::vec3 c(center.x + dx, center.y + dy, center.z + dz);

			triangle.push_back(v.x);
			triangle.push_back(v.y);
			triangle.push_back(v.z);
		}

		glGenVertexArrays(1, &TRIANGLECUBEVAO);
		glGenBuffers(1, &TRIANGLECUBEVBO);
		glGenBuffers(1, &TRIANGLECUBEEBO);
		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(TRIANGLECUBEVAO);

		glBindBuffer(GL_ARRAY_BUFFER, TRIANGLECUBEVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * triangle.size(), triangle.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TRIANGLECUBEEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * triangle_indices.size(), triangle_indices.data(), GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}

	void _initializeBorderedCube() {
		glGenVertexArrays(1, &BORDERCUBEVAO);
		glGenBuffers(1, &BORDERCUBEVBO);
		glGenBuffers(1, &BORDERCUBEEBO);
		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(BORDERCUBEVAO);

		glBindBuffer(GL_ARRAY_BUFFER, BORDERCUBEVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * bordered_cube.size(), bordered_cube.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BORDERCUBEEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * bordered_cube_indices.size(), bordered_cube_indices.data(), GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0); 
	}

	void _initializeSquare() {
		glGenVertexArrays(1, &SQUAREVAO);
		glGenBuffers(1, &SQUAREVBO);
		glGenBuffers(1, &SQUAREEBO);
		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(SQUAREVAO);

		glBindBuffer(GL_ARRAY_BUFFER, SQUAREVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * square.size(), square.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SQUAREEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * square_indices.size(), square_indices.data(), GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}

	void initializeShapeEngine() {
		_initializeBorderedCube();
		_initializeTriangle();
		_initializeSquare();
	}

	glm::mat4 createSquareWorldTransform(const Square& square)
	{
		glm::mat4 model;
		float scale = square._width / 1.0f;

		model = glm::translate(glm::vec3(square._top_left.x, square._top_left.y, square._width + square._top_left.z));
		model = glm::scale(model, glm::vec3(scale, 1.0f, scale));
		model = glm::rotate(model, glm::radians(square._pitch), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(square._yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(square._roll), glm::vec3(0.0f, 0.0f, 1.0f));
		return model;

	}

	void drawBorderedCube() {
		glBindVertexArray(BORDERCUBEVAO);
		glDrawElements(GL_LINES, bordered_cube_indices.size(), GL_UNSIGNED_INT, 0); //glDrawElements for indices, glDrawArrays for vertices
		glBindVertexArray(0);
	}

	void drawSquare() {
		glBindVertexArray(SQUAREVAO);
		glDrawElements(GL_LINES, square_indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

	void drawTriangle() {
		glBindVertexArray(TRIANGLECUBEVAO);
		glDrawElements(GL_TRIANGLES, triangle.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}


#pragma region CLEANUP
	void _destroyBorderedCube() {
		glDeleteVertexArrays(1, &BORDERCUBEVAO);
		glDeleteBuffers(1, &BORDERCUBEVBO);
		glDeleteBuffers(1, &BORDERCUBEEBO);
	}

	void _destroySquare() {
		glDeleteVertexArrays(1, &TRIANGLECUBEVAO);
		glDeleteBuffers(1, &TRIANGLECUBEVBO);
		glDeleteBuffers(1, &TRIANGLECUBEEBO);
	}

	void _destroyTriangle() {
		glDeleteVertexArrays(1, &TRIANGLECUBEVAO);
		glDeleteBuffers(1, &TRIANGLECUBEVBO);
		glDeleteBuffers(1, &TRIANGLECUBEEBO);
	}

	void destroyShapeEngine() {
		_destroyBorderedCube();
		_destroySquare();
		_destroyTriangle();
	}



#pragma endregion
}
