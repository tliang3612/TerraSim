#include "heightmap.h"


Heightmap::Heightmap(float size, int resolution, float noiseSeed, DataFactory dataFactory) : m_heightmapSize(size), m_heightmapResolution(resolution), m_noiseSeed(noiseSeed) {
    m_map = std::make_unique<float[]>(m_heightmapResolution * m_heightmapResolution);
    GenerateHeightsUsingNoise(0, noiseSeed);

    m_textureID = dataFactory.CreateTexture();
    glBindTexture(GL_TEXTURE_2D, m_textureID); // make heightmap texture configurable
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // prevent horizontal wrapping outside of [0,1]
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // same but for vertical
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // linearly interpolate texture values between neighbor textures to look smoother
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // same but for larger sample size
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_heightmapResolution, m_heightmapResolution, 0, GL_RED, GL_FLOAT, m_map.get()); // upload texture data to gpu
    glBindTexture(GL_TEXTURE_2D, 0);
}

//generates height values using noise
void Heightmap::GenerateHeightsUsingNoise(int noiseType, float noiseSeed) {
    if (noiseSeed != m_noiseSeed) {
        m_noise.SetSeed(noiseSeed);
        m_noiseSeed = noiseSeed;
    }

    if (noiseType == 0) {
        m_noise.SetNoiseType(FastNoise::SimplexFractal);
        m_noise.SetFractalOctaves(5);
    }
    else {
        m_noise.SetNoiseType(FastNoise::Simplex);
        m_noise.SetFractalOctaves(5);
    }

    m_maxHeight = FLT_MIN;
    m_minHeight = FLT_MAX;

    //init heightmap with noise values
    for (int i = 0; i < m_heightmapResolution; ++i) {
        for (int j = 0; j < m_heightmapResolution; ++j) {
            float x = float(i) / float(m_heightmapResolution - 1) * 2.f - 1.f;
            float y = float(j) / float(m_heightmapResolution - 1) * 2.f - 1.f;

            float height = 0.f;

            if (noiseType == 0) {
                height = SampleNoise(x * Frequency, y * Frequency) * Amplitude;
            }
            else {
                height = fBm(glm::vec2(x, y)) * Amplitude;
            }

            SetHeight(i, j, height);
        }
    }
}

const float Heightmap::GetHeight(int x, int z) const {
    if (x < 0 || x >= m_heightmapResolution ||
        z < 0 || z >= m_heightmapResolution) {
        return 0.f;
    }
    return m_map[z * m_heightmapResolution + x];
}

void Heightmap::SetSize(float size){
    m_heightmapSize = size;
    m_heightmapResolution = size * 2;
    m_map = std::make_unique<float[]>(m_heightmapResolution * m_heightmapResolution);
}

void Heightmap::SetHeight(int x, int z, float height)
{
    if (x < 0 || x >= m_heightmapResolution ||
        z < 0 || z >= m_heightmapResolution) {
        return;
    }
    if (height < m_minHeight) {
        m_minHeight = height;
    }
    if (height > m_maxHeight) {
        m_maxHeight = height;
    }
    m_map[z * m_heightmapResolution + x] = height;
}


// fbm params
float Heightmap::fBm(glm::vec2 position) {
    const int octaves = 5;           //number of fbm octaves
    const float lacunarity = 1.9f;  //freq multiplier per octave
    const float gain = 0.5f;        //amplitude multiplier per octave

    float accumulatedNoise = 0.f;  //final fbm value
    float amplitude = 0.5f;
    float freq = Frequency;
    float prev = 1.f; //previous noise val

    for (int i = 0; i < octaves; ++i) {
        float noise = SampleNoise(position.x * freq, position.y * freq);

        float ridgeNoise = 1.f - std::abs(noise);

        //smoothing functions for valleys and peaks
        if (ridgeNoise < .5f) {
            ridgeNoise = 4.f * glm::pow(ridgeNoise, 3); // valleys
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
    float linear = glm::clamp(1.f - distance / maxDistance, 0.f, 1.f); // create mountain ranges more towards the middle
    float falloff = linear* linear* (3 - 2 * linear); //smooth interpolation
    return accumulatedNoise * falloff;
}

float Heightmap::SampleNoise(float x, float y) {
    return m_noise.GetNoise(x * m_heightmapSize, y * m_heightmapSize);
}

void Heightmap::Update() {
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, m_heightmapResolution, m_heightmapResolution, 0, GL_RED, GL_FLOAT, m_map.get());
    glBindTexture(GL_TEXTURE_2D, 0);
}

