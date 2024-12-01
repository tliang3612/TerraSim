
#pragma once
#include <glad/glad.h>
#include <FastNoise/FastNoise.h>
#include <glm/glm.hpp>
#include <time.h>
#include <memory>
#include <iostream>
#include "data_factory.h"

class Heightmap {
public:
    Heightmap() = default;
    Heightmap(float size, int resolution, DataFactory dataFactory);

    //allows std move so terrain can take ownership
    Heightmap(Heightmap&&) noexcept = default;  
    Heightmap& operator=(Heightmap&&) noexcept = default; 

    void GenerateHeightsUsingNoise(int noiseType, bool setSeed);
    void Update();

    const GLuint GetTextureID() const { return m_textureID; }
    const float GetMinHeight() const { return m_minHeight; }
    const float GetMaxHeight() const { return m_maxHeight; }
    const float GetSize() const { return m_heightmapSize; }
    const float GetResolution() const { return m_heightmapResolution; }
    const float GetHeight(int x, int z) const;

    void SetMaxHeight(float maxHeight) { m_maxHeight = maxHeight; }
    void SetMinHeight(float minHeight) { m_maxHeight = minHeight; }
    void SetHeight(int x, int z, float height);

    float Amplitude = 80.f;
    float Frequency = 0.2f;

private:
    float fBm(glm::vec2 position);
    float SampleNoise(float x, float y);

    FastNoise m_noise;
    GLuint m_textureID;
    int m_heightmapResolution;
    std::unique_ptr<float[]> m_map;
    float m_heightmapSize;
    float m_maxHeight;
    float m_minHeight;
};