#pragma once
#include "heightmap.hpp"
#include "data_factory.h"

class Terrain {
public:
	Terrain(Model model, Heightmap heightmap, GLuint textureID);
	Terrain() = default;

	float GetHeightAtPoint(float x, float y);
	void SetHeightAtPoint(float x, float y, float height);
	void Update();
	void Destroy();
	const GLuint GetTextureID() { return m_textureID; }
	const Model GetModel() { return m_model; }
	const Heightmap GetHeightmap() { return m_heightmap; }



private:
	Model m_model;
	Heightmap m_heightmap;
	GLuint m_textureID;

};

class TerrainFactory {
public:
	TerrainFactory() = default;

	Terrain GenerateTerrain(DataFactory modelFactory, float size, int resolution, GLuint texture);
};