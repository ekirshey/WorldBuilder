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
		World::ChunksVertexCount chunksvertexct;
		std::vector<unsigned int> vertices;
		_selectionCircle._center = pickray; 
		if (_selectionCircle._center.y == 0.0f) {
			_selectionCircle._center.y = 0.01f;
		}

		if (keys[SDL_SCANCODE_J] || keys[SDL_SCANCODE_K]) {
			glm::vec3 change = glm::vec3(0.0f, 0.1f, 0.0f);
			if (keys[SDL_SCANCODE_K]) {
				change = glm::vec3(0.0f, -0.1f, 0.0f);
			}
			int lastcount = 0;
			world->ChunkVerticesInCircle(_selectionCircle, chunksvertexct, vertices);
			for (const auto& ct : chunksvertexct) {
				for (int i = lastcount; i < lastcount+ct.second; i++) {
					world->ModifyChunkVertex(ct.first, vertices[i], change);
				}
				lastcount += ct.second;
			}
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
