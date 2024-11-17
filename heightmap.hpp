#pragma once
#include <glad/glad.h>
#include <FastNoise/FastNoise.h>
#include <glm/glm.hpp>
#include <time.h>
#include <memory>
#include "data_factory.h"
#include <iostream>
#include <map>

const FastNoise::NoiseType noiseTypeArray[3] = {
	FastNoise::Perlin,
	FastNoise::SimplexFractal
};

struct Heightmap {
	FastNoise noise;
	GLuint textureID;
	int heightmapResolution;
	float* map;
	float heightmapSize;
	float maxHeight = 0;
	float minHeight = 1;

	// Noise parameters
	float frequency = .2f;
	float amplitude = 80.0f;

	Heightmap() = default;
	
	
	Heightmap(float size, int resolution, DataFactory dataFactory)
	{
		this->heightmapSize = size;
		this->heightmapResolution = resolution;
		map = new float[heightmapResolution * heightmapResolution]; //using raw pointer for map for simplicity
		GenerateHeightsUsingNoise(0);

		textureID = dataFactory.CreateTexture();
		glBindTexture(GL_TEXTURE_2D, textureID); //make heightmap texture configurable
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //prevent horizontal wrapping outside of [0,1]
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //same but for verticale
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//lineearly interpolate texture values between neighbor textures to look smoother
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // same but for larger sample size                                      
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, heightmapResolution, heightmapResolution, 0, GL_RED, GL_FLOAT, map); //upload texture data to gpu
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void GenerateHeightsUsingNoise(int noiseType) {
		noise.SetSeed(time(NULL));
		noise.SetNoiseType(noiseTypeArray[1]);
		noise.SetFractalOctaves(5);

		//Init heightmap with noise values
		for (int i = 0; i < heightmapResolution; i++) {
			for (int j = 0; j < heightmapResolution; j++) {
				float x = float(i) / float(heightmapResolution - 1) * 2.0f - 1.0f;
				float y = float(j) / float(heightmapResolution - 1) * 2.0f - 1.0f;

				//int octaves = 5;
				//float gain = 0.6f; 
				//float lacunarity = 2.0f;
				//float baseHeight = fBm(glm::vec2(x,y), octaves, gain, lacunarity);

				//float perlinNoise = baseHeight;
				//float billowNoise = abs(perlinNoise);
				//float ridgeNoise = 1.0f - billowNoise;

				//float Ss = -.5f;
				//float Se = .5f;
				//float Sh = 30.0f;				
				//float combinedNoise = (Ss >= 0.0f) ? glm::mix(perlinNoise, billowNoise, abs(Ss)) : glm::mix(perlinNoise, ridgeNoise, Ss);

				//float height = combinedNoise * pow(abs(combinedNoise), Se) * Sh;
				float height = SampleNoise(x * frequency, y * frequency) * amplitude;
				map[j * heightmapResolution + i] = height; 
				if (map[j * heightmapResolution + i] < minHeight) {
					minHeight = map[j * heightmapResolution + i];
				}
				if (map[j * heightmapResolution + i] > maxHeight) {
					maxHeight = map[j * heightmapResolution + i];
				}
			}
		}
	}

	float fBm(glm::vec2 p, int octaves, float gain, float lacunarity) {
		float amplitude = 1.f;
		float frequency = 0.5f;
		float sum = 0.0;

		for (int i = 0; i < octaves; ++i) {
			sum += amplitude * SampleNoise(p.x * frequency, p.y * frequency);
			amplitude *= gain;      
			frequency *= lacunarity;
		}

		return sum;
	}

	float SampleNoise(float x, float y)
	{
		return noise.GetNoise(x * heightmapSize, y * heightmapSize);
	}

	//delete the map raw pointer
	void Destroy() {
		delete[] map;
	}

	void Update()
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, heightmapResolution, heightmapResolution, 0, GL_RED, GL_FLOAT, map);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

};