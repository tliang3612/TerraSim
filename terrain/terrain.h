#pragma once
#include <vector>
#include "heightmap.h"
#include "../engine/data_factory.h"
#include "../effects/shadowmap.hpp"

class Terrain {
public:
	Terrain(Model model, std::shared_ptr<Heightmap> heightmap, Shadowmap shadowmap, std::vector<GLuint> textureIDs, std::vector<float> textureCoords, std::vector<float> vertices, std::vector<int> indices);
	Terrain() = default;

	void Update();
	void UpdateTexture(int index, GLuint newTextureID);
	void UpdateSize(float size);
	const float GetHeightFromWorld(int x, int z) const;
	const float GetMinHeight() { return m_heightmap->GetMinHeight(); }
	const float GetMaxHeight() { return m_heightmap->GetMaxHeight(); }
	const Model GetModel() const { return m_model; }
	std::shared_ptr<Heightmap> GetHeightmap() const { return m_heightmap; }
	Shadowmap& GetShadowmap() { return m_shadowmap; }
	const std::vector<GLuint> GetTextureIDs() const { return m_textureIDs; }
	const std::vector<float> GetVeritices() const { return m_vertices;}
	const std::vector<float> GetTextureCoords() const { return m_textureCoords; }
	const std::vector<int> GetIndices() const { return m_indices; }

private:
	Model m_model;
	std::shared_ptr<Heightmap> m_heightmap;
	Shadowmap m_shadowmap;
	std::vector<GLuint> m_textureIDs;
	std::vector<float> m_vertices;
	std::vector<float> m_textureCoords;
	std::vector<int> m_indices;
};

class TerrainFactory {
public:
	TerrainFactory() = default;

	Terrain GenerateTerrain(DataFactory dataFactory, float size, int resolution, std::vector<GLuint> textureIDs, float noiseSeed);
};