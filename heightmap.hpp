#pragma once
#include <glad/glad.h>
#include <FastNoise/FastNoise.h>
#include <glm/glm.hpp>
#include <time.h>
#include <memory>
#include "data_factory.h"
#include <iostream>
struct Heightmap {
	FastNoise noise;
	GLuint textureID;
	float* map;
	float heightmapSize;
	int heightmapResolution;
	float scale = 0.2f;
	float amplitude = 80.0f;

	Heightmap() = default;
	
	
	Heightmap(float size, int resolution, DataFactory dataFactory)
	{
		this->heightmapSize = size;
		this->heightmapResolution = resolution;
		map = new float[heightmapResolution * heightmapResolution]; //using raw pointer for map for simplicity
		noise.SetNoiseType(FastNoise::SimplexFractal);
		noise.SetFractalOctaves(6);
		noise.SetSeed(time(NULL));

		float minHeight = 0;
		float maxHeight = 1;
		//Init heightmap with noise values
		for (int i = 0; i < heightmapResolution; i++) {
			for (int j = 0; j < heightmapResolution; j++) {
				float x = float(i) / float(heightmapResolution - 1) * 2.0f - 1.0f;
				float y = float(j) / float(heightmapResolution - 1) * 2.0f - 1.0f;
				map[j * heightmapResolution + i] = SampleNoise(x * heightmapSize, y * heightmapSize);
				if (map[j * heightmapResolution + i] < minHeight) {
					minHeight = map[j * heightmapResolution + i];
				}
				if (map[j * heightmapResolution + i] > maxHeight) {
					maxHeight = map[j * heightmapResolution + i];
				}
			}
		}
		std::cout << minHeight << std::endl;
		std::cout << maxHeight << std::endl;


		textureID = dataFactory.CreateTexture();
		glBindTexture(GL_TEXTURE_2D, textureID); //make heightmap texture configurable
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //prevent horizontal wrapping outside of [0,1]
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //same but for verticale
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//lineearly interpolate texture values between neighbor textures to look smoother
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // same but for larger sample size                                      
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, heightmapResolution, heightmapResolution, 0, GL_RED, GL_FLOAT, map); //upload texture data to gpu
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	float SampleNoise(float x, float y)
	{
		return noise.GetNoise(x * scale, y * scale) * amplitude;
	}

	//delete the map raw pointer
	void Destroy() {
		delete[] map;
	}

	void Update()
	{
		//TODO
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, heightmapResolution, heightmapResolution, 0, GL_RED, GL_FLOAT, map);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

};