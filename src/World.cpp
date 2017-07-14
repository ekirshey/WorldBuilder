#include "World.h"
#include <glm/gtc/type_ptr.hpp>

World::World()
{
}

void World::AddChunk(glm::vec3 position, GLfloat width, int rows, int cols)
{
	_models.push_back(chunk::Model(position, width, rows, cols));
	_chunks.push_back(WorldChunk(_models.size() - 1, chunk::Geometry(position, width)));
}

int World::GetSelectedChunk(const Ray & ray, float& intersect_point)
{
	for (int i = 0; i < _chunks.size(); i++) {
		if (_chunks[i].intersectsWithRay(ray, intersect_point)) {
			return i;
		}
	}

	return -1;
}

void World::DrawWorld(ShaderProgram shader, const glm::mat4& view, const glm::mat4& projection)
{
	shader.useProgram();

	GLint viewLoc = shader.getUniformLocation("view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	GLint projectionLoc = shader.getUniformLocation("projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	for (auto& c : _chunks) {
		glm::mat4 model;
		c.buildModelMatrix(model);

		GLint modelLoc = shader.getUniformLocation("model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		_models[c.modelId()].draw();
	}
}

bool World::ChunkVertexIntersectsWithRay(int chunkid, const Ray & ray, float intersect_point, unsigned int& vertex_id)
{
	
	if (chunkid < 0 || chunkid > _chunks.size()) {
		return false;
	}

	int modelid = _chunks[chunkid].modelId();
	if (!_models[modelid].vertexIntersectsWithRay(ray, intersect_point, vertex_id)) {
		return false;
	}

	return true;

}

bool World::ChunkFaceIntersectsWithRay(int chunkid, const Ray & ray, unsigned int& face_index)
{
	if (chunkid < 0 || chunkid > _chunks.size()) {
		return false;
	}

	int modelid = _chunks[chunkid].modelId();
	if (!_models[modelid].faceIntersectsWithRay(ray, face_index)) {
		return false;
	}

	return true;
}



void World::ModifyChunkVertex(int chunkid, int vertex, const glm::vec3& change) {
	int modelid = _chunks[chunkid].modelId();
	_models[modelid].modifyVertex(vertex, change);
}

void World::ModifyChunkFace(int chunkid, int face, const glm::vec3& change)
{
	int modelid = _chunks[chunkid].modelId();
	_models[modelid].modifyFace(face, change);
}


void World::ReloadChunk(int chunkid) {
	int modelid = _chunks[chunkid].modelId();
	_models[modelid].reloadVertexData();
}



