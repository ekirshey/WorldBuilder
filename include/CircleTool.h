#pragma once
#include "World.h"
#include "ShapePrimitives.h"

class CircleTool {
	public:
		CircleTool();

		void setActive() { _active = true; }
		void setInactive() { _active = false; }
		void setRadius(float radius) { _selectionCircle._radius = radius; }

		void update(World* world, const glm::vec3& pickray, bool* keys);
		void draw(ShaderProgram shader, const glm::mat4& view, const glm::mat4& projection);
	private:
		shapes::Circle _selectionCircle;
		glm::mat4 _circleModel;
		bool _active;
};
