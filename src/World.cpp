#include "World.h"
#include <glm/gtc/type_ptr.hpp>

World::World()
{
}

void World::AddChunk(glm::vec3 localcoords, glm::vec3 worldtransform, GLfloat width, int rows, int cols)
{
	_models.push_back(chunk::Model(localcoords,width, rows, cols));
	_chunks.push_back(WorldChunk(_models.size() - 1, chunk::Geometry(localcoords, worldtransform, width)));
}

int World::GetSelectedChunk(const Ray & ray, float& intersect_point)
{
	for (int i = 0; i < _chunks.size(); i++) {
		if (_chunks[i].geometry.intersectsWithRay(ray, intersect_point)) {
			return i;
		}
	}

	return -1;
}

void World::DrawWorld(ShaderProgram shader, const glm::mat4& view, const glm::mat4& projection)
{
	for (auto& id : _modifiedChunks) {
		_reloadChunk(id);
	}
	_modifiedChunks.clear();

	shader.useProgram();

	GLint viewLoc = shader.getUniformLocation("view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	GLint projectionLoc = shader.getUniformLocation("projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	for (auto& c : _chunks) {
		glm::mat4 model;
		c.geometry.buildModelMatrix(model);

		GLint modelLoc = shader.getUniformLocation("model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		_models[c.modelid].draw();
	}
}

bool World::ChunkVertexIntersectsWithRay(int chunkid, const Ray & ray, float intersect_point, unsigned int& vertex_id)
{
	
	if (chunkid < 0 || chunkid > _chunks.size()) {
		return false;
	}

	int modelid = _chunks[chunkid].modelid;
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

	int modelid = _chunks[chunkid].modelid;
	if (!_models[modelid].faceIntersectsWithRay(ray, face_index)) {
		return false;
	}

	return true;
}

World::ChunksIndices World::ChunkIndicesInCube( GLfloat leftbound,
												GLfloat rightbound,
												GLfloat topbound,
												GLfloat bottombound,
												GLfloat frontbound,
												GLfloat backbound) 
{
	ChunksIndices chunks_indices;
	bool surrounds;
	for (int i = 0; i < _chunks.size(); i++) {
		if (_chunks[i].geometry.intersectsWithCube(leftbound, rightbound, frontbound, backbound, surrounds)) {
			std::vector<unsigned int> chunkindices;
			// Optimization: If you the box surrounds the chunk entirely grab all the vectors
			if (surrounds) {
				_models[_chunks[i].modelid].addIndicesToVector(chunkindices);
			}
			else {
				// Need to convert the bounds to local model coords
				glm::vec4 minBounds(leftbound, topbound, backbound, 1.0f);
				glm::vec4 maxBounds(rightbound, bottombound, frontbound, 1.0f);
				_chunks[i].geometry.vectorToChunkLocalCoords(minBounds);
				_chunks[i].geometry.vectorToChunkLocalCoords(maxBounds);

				_models[_chunks[i].modelid].indicesInCube(chunkindices,
					minBounds.x,
					maxBounds.x,
					minBounds.y,
					maxBounds.y,
					maxBounds.z,
					minBounds.z);
			}
			chunks_indices.insert({ i, chunkindices } );
		}
	}

	return chunks_indices;
}

void World::ModifyChunkVertices(int chunkid, const std::vector<unsigned int> vertices, const glm::vec3& change) {
	int modelid = _chunks[chunkid].modelid;

	for (const auto& v : vertices) {
		_models[modelid].modifyVertex(v, change);
	}

	_modifiedChunks.push_back(chunkid);
}

void World::ModifyChunkVertex(int chunkid, unsigned int vertex, const glm::vec3& change) {
	int modelid = _chunks[chunkid].modelid;
	_models[modelid].modifyVertex(vertex, change);

	_modifiedChunks.push_back(chunkid);
}

void World::ModifyChunkFace(int chunkid, int face, const glm::vec3& change)
{
	int modelid = _chunks[chunkid].modelid;
	_models[modelid].modifyFace(face, change);

	_modifiedChunks.push_back(chunkid);
}


void World::_reloadChunk(int chunkid) {
	int modelid = _chunks[chunkid].modelid;
	_models[modelid].reloadVertexData();
}



