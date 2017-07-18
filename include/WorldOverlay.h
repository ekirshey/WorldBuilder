#pragma once
#include "Shader.h"
#include "World.h"

class WorldOverlay {
	public:
		enum class SelectionMode {CHUNK=0, FACE, VERTEX};

		WorldOverlay();

		void setSelectionMode(int mode);
		void setFocusedChunk(int id);
		void setFocusedFace(int id);
		void setFocusedVertex(int id);


		void drawOverlay(ShaderProgram& shader, const World& world, const glm::vec3 raypos, const glm::mat4& view, const glm::mat4& projection);

	private:

		void _chunkOvelay(ShaderProgram& shader, const World& world, const glm::vec3& raypos);
		void _faceOverlay(ShaderProgram& shader, const World& world, const glm::vec3& raypos);
		void _vertexOverlay(ShaderProgram& shader, const World& world, const glm::vec3& raypos);
		SelectionMode _selectionMode;
		int _focusedChunk;
		int _focusedFace;
		int _focusedVertex;
		glm::vec3 _lastRayPos;
		glm::mat4 _chunkModel;
		glm::mat4 _vertexModel;


};
