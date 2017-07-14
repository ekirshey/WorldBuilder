#pragma once
#include <vector>
#include "ChunkModel.h"
#include "ChunkGeometry.h"
#include "Shader.h"

class WorldChunk {
	public:
		WorldChunk(int modelid, const chunk::Geometry& geometry) 
			: _modelid(modelid)
			, _geometry(geometry)
		{

		}

		bool intersectsWithRay(const Ray& ray) {
			return _geometry.intersectsWithRay(ray);
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
		int GetSelectedChunk(const Ray& ray);
		void DrawWorld(ShaderProgram shader, const glm::mat4& view, const glm::mat4& projection);
	private:
		std::vector<chunk::Model> _models;
		std::vector<WorldChunk> _chunks;

};