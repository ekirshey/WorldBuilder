#pragma once
#include <vector>
#include <unordered_map>
#include "ChunkModel.h"
#include "ChunkGeometry.h"
#include "Shader.h"

// Essentially a pair/tuple with a wrapper for semantic sugar
struct WorldChunk {
		WorldChunk(int modelid, const chunk::Geometry& geometry) 
			: modelid(modelid)
			, geometry(geometry)
		{

		}

		int modelid;
		chunk::Geometry geometry;
};

class World {
	public:
		using ChunksIndices = std::unordered_map<int, std::vector<unsigned int>>;
		World();
		void AddChunk(glm::vec3 localcoords, glm::vec3 worldtransform, GLfloat width, int rows, int cols);
		int GetSelectedChunk(const Ray& ray, float& intersect_point);
		void DrawWorldOverlay(ShaderProgram shader, const glm::mat4& view, const glm::mat4& projection);
		void DrawWorld(ShaderProgram shader, const glm::mat4& view, const glm::mat4& projection);

		bool ChunkVertexIntersectsWithRay(int chunkid, const Ray& ray, float intersect_point, unsigned int& vertex_id);
		bool ChunkFaceIntersectsWithRay(int chunkid, const Ray& ray, float intersect_point, unsigned int& face_index);
		ChunksIndices ChunkIndicesInCube( GLfloat leftbound,
										  GLfloat rightbound,
										  GLfloat topbound,
										  GLfloat bottombound,
										  GLfloat frontbound,
										  GLfloat backbound);

		void ModifyChunkVertex( int chunkid, unsigned int vertex, const glm::vec3& change);
		void ModifyChunkVertices(int chunkid, const std::vector<unsigned int> vertices, const glm::vec3 & change);
		void ModifyChunkFace( int chunkid, int face, const glm::vec3& change);

	private:
		void _reloadChunk(int chunkid);
		std::vector<chunk::Model> _models;
		std::vector<WorldChunk> _chunks;
		std::vector<unsigned int> _modifiedChunks;
		int _focusedChunk;
};