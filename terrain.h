#pragma once
#include "heightmap.hpp"
#include "data_factory.h"

class Terrain {
public:
	Terrain(Model model, Heightmap heightmap, std::vector<GLuint> textureIDs);
	Terrain() = default;

	void SetHeightAtPoint(float x, float y, float height);
	void Update();
	void UpdateTexture(int index, GLuint newTextureID);
	void Destroy();
	void AddHeight(float u, float v, float radius, float height);
	const float GetHeightFromWorld(int x, int z);
	const float GetMinHeight() { return m_heightmap.minHeight; }
	const float GetMaxHeight() { return m_heightmap.maxHeight; }
	const std::vector<GLuint> GetTextureIDs() { return m_textureIDs; }
	const Model GetModel() { return m_model; }
	Heightmap& GetHeightmap() { return m_heightmap; }



private:
	Model m_model;
	Heightmap m_heightmap;
	std::vector<GLuint> m_textureIDs;

};

class TerrainFactory {
public:
	TerrainFactory() = default;

	Terrain GenerateTerrain(DataFactory modelFactory, float size, int resolution, std::vector<GLuint> textureIDs);
};