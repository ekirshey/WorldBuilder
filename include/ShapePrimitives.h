#pragma once
#include "Shader.h"
#include <glm/glm.hpp>

namespace shapes {
	void initializeShapeEngine();
	void destroyShapeEngine();

	// Draw calls
	void drawBorderedCube();
	void drawTriangle();
	// Draw using loaded shader and model matrix
	void drawSquare();
	// Use a specific shader and values
	void drawSquare(GLint modelloc, glm::vec3 topleft, glm::vec3 bottomright);
}

