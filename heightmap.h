#pragma once
#include <glad/glad.h>
#include <FastNoise/FastNoise.h>
#include <glm/glm.hpp>
#include <time.h>
#include <memory>
#include "model_factory.h"
struct Heightmap {
	FastNoise noise;
	GLuint textureID;
	float* map;
	float heightmapSize;
	int heightmapResolution;
	float scale = 0.2f;
	float amplitude = 80.0f;
	
	Heightmap() = default;
	Heightmap(float size, int resolution, ModelFactory modelFactory);

	float SampleNoise(float x, float y);

	void Update();
	void Destroy();
};