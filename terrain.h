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
	const float GetHeight(int x, int z) { return m_heightmap.map[z + m_heightmap.heightmapResolution + x]; }
	const float GetMinHeight() { return m_heightmap.minHeight; }
	const float GetMaxHeight() { return m_heightmap.maxHeight; }
	const std::vector<GLuint> GetTextureIDs() { return m_textureIDs; }
	const Model GetModel() { return m_model; }
	const Heightmap GetHeightmap() { return m_heightmap; }



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