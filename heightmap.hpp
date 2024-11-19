#pragma once
#include <glad/glad.h>
#include <FastNoise/FastNoise.h>
#include <glm/glm.hpp>
#include <time.h>
#include <memory>
#include "data_factory.h"
#include <iostream>
#include <map>

struct Heightmap {
	FastNoise noise;
	GLuint textureID;
	int heightmapResolution;
	float* map;
	float heightmapSize;
	float maxHeight = 0;
	float minHeight = 1;

	// Noise parameters
	float amplitude = 80.f;
	float frequency = .5f;


	Heightmap() = default;
	
	
	Heightmap(float size, int resolution, DataFactory dataFactory)
	{
		this->heightmapSize = size;
		this->heightmapResolution = resolution;
		map = new float[heightmapResolution * heightmapResolution]; //using raw pointer for map for simplicity
		GenerateHeightsUsingNoise(0, true);

		textureID = dataFactory.CreateTexture();
		glBindTexture(GL_TEXTURE_2D, textureID); //make heightmap texture configurable
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //prevent horizontal wrapping outside of [0,1]
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //same but for verticale
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//lineearly interpolate texture values between neighbor textures to look smoother
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // same but for larger sample size                                      
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, heightmapResolution, heightmapResolution, 0, GL_RED, GL_FLOAT, map); //upload texture data to gpu
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void GenerateHeightsUsingNoise(int noiseType, bool setSeed) {

		if (setSeed) {
			noise.SetSeed(time(NULL));
		}

		if (noiseType == 0) {
			noise.SetNoiseType(FastNoise::SimplexFractal);
			noise.SetFractalOctaves(5);
		}
		else {
			noise.SetNoiseType(FastNoise::Simplex);
		}

		//Init heightmap with noise values
		for (int i = 0; i < heightmapResolution; i++) {
			for (int j = 0; j < heightmapResolution; j++) {
				float x = float(i) / float(heightmapResolution - 1) * 2.0f - 1.0f;
				float y = float(j) / float(heightmapResolution - 1) * 2.0f - 1.0f;

				float height = 0.f;

				if (noiseType == 0) {
					height = SampleNoise(x * frequency, y * frequency) * amplitude;
				}
				else {
					height = fBm(glm::vec2(x, y)) * amplitude;
				}
				
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

	float fBm(glm::vec2 position) {
		//fbm params
		const int octaves = 5;           
		const float lacunarity = 1.9f;    
		const float gain = 0.5f;          

		float accumulatedNoise = 0.0f;  
		float amplitude = 0.5f;
		float freq = frequency;
		float prev = 1.0f; //previous noise val

		for (int i = 0; i < octaves; i++) {
			float noise = SampleNoise(position.x * freq, position.y * freq);

			float ridgeNoise = 1.f - std::abs(noise);

			//smoothing functions for valleys and peaks
			if (ridgeNoise < .5f) {
				ridgeNoise = 4.f * glm::pow(ridgeNoise, 3); //valleys 
			}
			else {
				ridgeNoise = (ridgeNoise - 1.f) * glm::pow((2.f * ridgeNoise - 2.f), 2) + 1.f; //peaks 
			}

			accumulatedNoise += ridgeNoise * amplitude * prev;

			prev = ridgeNoise;

			//adjust for next octave
			freq *= lacunarity; 
			amplitude *= gain;  
		}
		float maxDistance = std::sqrt(2.f);
		float distance = glm::length(position);
		float falloff = glm::clamp(1.f - distance / maxDistance, 0.f, 1.f); // create a mountain ranges more towards the middle
		return accumulatedNoise * falloff;
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