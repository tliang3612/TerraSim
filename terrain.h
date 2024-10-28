#pragma once
#include "heightmap.hpp"
#include "model_factory.h"

class Terrain {
public:
	Terrain(Model model, Heightmap heightmap);
	Terrain() = default;

	float GetHeightAtPoint(float x, float y);
	void SetHeightAtPoint(float x, float y, float height);
	void Update();
	void Destroy();
	const Model GetModel() { return m_model; }
	const Heightmap GetHeightmap() { return m_heightmap; }



private:
	Model m_model;
	Heightmap m_heightmap;

};

class TerrainFactory {
public:
	TerrainFactory() = default;

	Terrain GenerateTerrain(ModelFactory modelFactory, float size, int resolution);
};