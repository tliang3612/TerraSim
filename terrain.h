#pragma once
#include "heightmap.hpp"
#include "data_factory.h"

class Terrain {
public:
	Terrain(Model model, Heightmap heightmap, std::vector<GLuint> textureIDs);
	Terrain() = default;

	float GetHeightAtPoint(float x, float y);
	void SetHeightAtPoint(float x, float y, float height);
	void Update();
	void UpdateTexture(int index, GLuint newTextureID);
	void Destroy();
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