#include "glm/gtc/type_ptr.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include "WorldOverlay.h"
#include "ShapePrimitives.h"

WorldOverlay::WorldOverlay() 
	: _selectionMode(SelectionMode::CHUNK)
	, _focusedChunk(-1)
	, _focusedFace(-1)
	, _focusedVertex(-1)
{

}

void WorldOverlay::setSelectionMode(int mode)
{
	_selectionMode = static_cast<SelectionMode>(mode);
}

void WorldOverlay::setFocusedChunk(int id) { 
	_focusedChunk = id; 
}

void WorldOverlay::setFocusedFace(int id) { 
	_focusedFace = id; 
}

void WorldOverlay::setFocusedVertex(int id) { 
	_focusedVertex = id; 
}

void WorldOverlay::_chunkOvelay(ShaderProgram& shader, const World& world, const glm::vec3& raypos) {
	auto position = world.getChunkPosition(_focusedChunk);
	position.y = 0.01;
	shapes::Square selectionBox(position, world.getChunkWidth(), 90.0f);
	if (_lastRayPos != raypos) {
		_chunkModel = shapes::createSquareWorldTransform(selectionBox);
	}
	GLint modelLoc = shader.getUniformLocation("model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(_chunkModel));

	shapes::drawSquare();
}

void WorldOverlay::_faceOverlay(ShaderProgram& shader, const World& world, const glm::vec3& raypos) {

}

void WorldOverlay::_vertexOverlay(ShaderProgram& shader, const World& world, const glm::vec3& raypos) {
	glm::vec3 position;

	// If it's a valid vertex position, draw a selection circle
	if (world.getChunkVertexPosition(_focusedChunk, _focusedVertex, position)) {
		auto chunkPosition = world.getChunkPosition(_focusedChunk);
		//position = raypos;
		position.y = 0.01;
		shapes::Circle selectionCircle(chunkPosition + position, 0.01f, 90.0f);
		if (_lastRayPos != raypos) {
			_vertexModel = shapes::createCircleWorldTransform(selectionCircle);
		}
		GLint modelLoc = shader.getUniformLocation("model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(_vertexModel));

		shapes::drawCircle();
	}
}

void WorldOverlay::drawOverlay(ShaderProgram& shader, const World& world, const glm::vec3 raypos, const glm::mat4 & view, const glm::mat4 & projection)
{

	shader.useProgram();

	GLint viewLoc = shader.getUniformLocation("view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	GLint projectionLoc = shader.getUniformLocation("projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	if (_selectionMode == SelectionMode::CHUNK && _focusedChunk != -1) {
		_chunkOvelay(shader, world, raypos);
	}
	else if (_selectionMode == SelectionMode::FACE && _focusedFace != -1) {
		_faceOverlay(shader, world, raypos);
	}
	else if (_selectionMode == SelectionMode::VERTEX && _focusedVertex != -1) {
		_vertexOverlay(shader, world, raypos);
	}

	_lastRayPos = raypos;
}
