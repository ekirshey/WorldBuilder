#pragma once
#include <vector>
#include <unordered_map>
#include "ChunkModel.h"
#include "ChunkGeometry.h"
#include "Shader.h"
#include "ShapePrimitives.h"

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
		World(float chunkwidth);
		void AddChunk(glm::vec3 localcoords, glm::vec3 worldtransform, int rows, int cols);
		int GetSelectedChunk(const Ray& ray, float& intersect_point);
		void DrawWorld(ShaderProgram shader, const glm::mat4& view, const glm::mat4& projection);

		bool ChunkVertexIntersectsWithRay(int chunkid, const Ray& ray, float intersect_point, unsigned int& vertex_id);
		bool ChunkFaceIntersectsWithRay(int chunkid, const Ray& ray, float intersect_point, unsigned int& face_index);
		ChunksIndices ChunkIndicesInCube( GLfloat leftbound,
										  GLfloat rightbound,
										  GLfloat topbound,
										  GLfloat bottombound,
										  GLfloat frontbound,
										  GLfloat backbound);
		bool ChunkVerticesInCircle( const shapes::Circle& circle, std::vector<unsigned int>& vertices);

		void ModifyChunkVertex( int chunkid, unsigned int vertex, const glm::vec3& change);
		void ModifyChunkVertices(int chunkid, const std::vector<unsigned int> vertices, const glm::vec3 & change);
		void ModifyChunkFace( int chunkid, int face, const glm::vec3& change);

		glm::vec3 getChunkPosition(int chunkid) const { return _chunks[chunkid].geometry.currentPosition(); }
		float getChunkWidth() const { return _chunkWidth; }

		bool getChunkVertexPosition(int chunkid, int vertexid, glm::vec3& vertexposition) const;
		bool getChunkFacePositions(int chunkid, int indexid, std::vector<glm::vec3>& facepositions) const;

	private:
		void _reloadChunk(int chunkid);
		std::vector<chunk::Model> _models;
		std::vector<WorldChunk> _chunks;
		float _chunkWidth;
		std::vector<unsigned int> _modifiedChunks;
};