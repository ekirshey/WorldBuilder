#include "CircleTool.h"
#include "glm/gtc/type_ptr.hpp"
#include <SDL.h>

CircleTool::CircleTool() 
	: _active(false)
	, _selectionCircle(glm::vec3(),0.5f,90.0f)
{

}

void CircleTool::update(World * world, const glm::vec3& pickray, bool* keys)
{
	if (_active) {
		std::vector<unsigned int> vertices;
		_selectionCircle._center = pickray; 
		if (_selectionCircle._center.y == 0.0f) {
			_selectionCircle._center.y = 0.01f;
		}

		if (keys[SDL_SCANCODE_J]) {
			world->ChunkVerticesInCircle(_selectionCircle, vertices);
			for (const auto& v : vertices) {
				world->ModifyChunkVertex(0, v, glm::vec3(0.0f, 0.01f, 0.0f));
			}
		}
		else if (keys[SDL_SCANCODE_K]) {
		}
	}
}

void CircleTool::draw(ShaderProgram shader, const glm::mat4& view, const glm::mat4& projection)
{
	if (_active) {
		_circleModel = shapes::createCircleWorldTransform(_selectionCircle);
		GLint modelLoc = shader.getUniformLocation("model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(_circleModel));

		shapes::drawCircle();
	}
}
