#pragma once
#include <vector>
#include "Shader.h"
#include <glm/glm.hpp>

namespace shapes {
	struct Square {
		Square() {}
		Square(glm::vec3 top_left, float width, float pitch = 0, float yaw = 0, float roll = 0) 
			: _top_left(top_left)
			, _width(width)
			, _pitch(pitch)
			, _yaw(yaw)
			, _roll(roll)
		{}

		glm::vec3 _top_left;
		float _width;
		float _yaw;
		float _pitch;
		float _roll;
	};

	struct Circle {
		Circle() {}
		Circle(glm::vec3 center, float radius, float pitch = 0, float yaw = 0, float roll = 0)
			: _center(center)
			, _radius(radius)
			, _pitch(pitch)
			, _yaw(yaw)
			, _roll(roll)
		{

		}

		glm::vec3 _center;
		float _radius;
		float _yaw;
		float _pitch;
		float _roll;
	};

	void initializeShapeEngine();
	void destroyShapeEngine();

	glm::mat4 createSquareWorldTransform(const Square& square);
	glm::mat4 createCircleWorldTransform(const Circle& circle);

	// Temp
	void loadVertexTriangleData(const std::vector<GLfloat>& triangle);

	// Draw calls
	void drawBorderedCube();
	void drawSquare();
	void drawTriangle();
	void drawCircle();
	void drawVertexTriangle();

	
}

