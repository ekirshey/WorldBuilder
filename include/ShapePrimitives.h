#pragma once
#include "Shader.h"
#include <glm/glm.hpp>

namespace shapes {
	struct Square {
		Square() {}
		Square(glm::vec3 top_left, float width, float pitch = 0, float yaw = 0, float roll = 0) 
			: _top_left(top_left)
			, _width(width)
			, _yaw(yaw)
			, _pitch(pitch)
			, _roll(roll)
		{}

		glm::vec3 _top_left;
		float _width;
		float _yaw;
		float _pitch;
		float _roll;
	};

	void initializeShapeEngine();
	void destroyShapeEngine();

	glm::mat4 createSquareWorldTransform(const Square& square);

	// Draw calls
	void drawBorderedCube();
	void drawTriangle();
	// Draw using loaded shader and model matrix
	void drawSquare();

	
}

