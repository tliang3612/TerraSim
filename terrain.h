#pragma once
#include "heightmap.hpp"
#include "data_factory.h"
#include "shadowmap.hpp"

class Terrain {
public:
	Terrain(Model model, Heightmap heightmap, Shadowmap shadowmap, std::vector<GLuint> textureIDs);
	Terrain() = default;

	void SetHeightAtPoint(float x, float y, float height);
	void Update();
	void UpdateTexture(int index, GLuint newTextureID);
	void Destroy();
	const float GetHeightFromWorld(int x, int z);
	const float GetMinHeight() { return m_heightmap.minHeight; }
	const float GetMaxHeight() { return m_heightmap.maxHeight; }
	const std::vector<GLuint> GetTextureIDs() { return m_textureIDs; }
	const Model GetModel() { return m_model; }
	Heightmap& GetHeightmap() { return m_heightmap; }
	Shadowmap& GetShadowmap() { return m_shadowmap; }



private:
	Model m_model;
	Heightmap m_heightmap;
	Shadowmap m_shadowmap;
	std::vector<GLuint> m_textureIDs;

};

class TerrainFactory {
public:
	TerrainFactory() = default;

	Terrain GenerateTerrain(DataFactory dataFactory, float size, int resolution, std::vector<GLuint> textureIDs);
};