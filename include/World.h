#pragma once
#include <vector>
#include "ChunkModel.h"
#include "ChunkGeometry.h"
#include "Shader.h"

// I think i can replace this class with a pair
// using WorldChunk = std::pair<int, chunk::Geometry>;
class WorldChunk {
	public:
		WorldChunk(int modelid, const chunk::Geometry& geometry) 
			: _modelid(modelid)
			, _geometry(geometry)
		{

		}

		bool intersectsWithRay(const Ray& ray, float& intersect_point) {
			return _geometry.intersectsWithRay(ray, intersect_point);
		}

		void buildModelMatrix(glm::mat4& model) {
			_geometry.buildModelMatrix(model);
		}

		int modelId() { return _modelid; }

	private:
		int _modelid;
		chunk::Geometry _geometry;
};

class World {
	public:
		World();
		void AddChunk(glm::vec3 position, GLfloat width, int rows, int cols);
		int GetSelectedChunk(const Ray& ray, float& intersect_point);
		void DrawWorld(ShaderProgram shader, const glm::mat4& view, const glm::mat4& projection);

		bool ChunkVertexIntersectsWithRay(int chunkid, const Ray& ray, float intersect_point, unsigned int& vertex_id);
		bool ChunkFaceIntersectsWithRay(int chunkid, const Ray& ray, unsigned int& face_index);

		void ModifyChunkVertex( int chunkid, int vertex, const glm::vec3& change);
		void ModifyChunkFace( int chunkid, int face, const glm::vec3& change);

		void ReloadChunk(int chunkid);
	private:
		std::vector<chunk::Model> _models;
		std::vector<WorldChunk> _chunks;

};