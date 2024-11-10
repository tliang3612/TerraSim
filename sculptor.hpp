#include "terrain.h"

struct Sculptor {

    static void Sculpt(Heightmap& heightmap, float pointX, float pointZ, float radius, float strength, int brushType) {
        float minRadius = radius/3;

        float normalizedX = (pointX / heightmap.heightmapSize + 1.0f); //[0, 1]
        float normalizedZ = (pointZ / heightmap.heightmapSize + 1.0f);

        int gridX = int(normalizedX/2.f * heightmap.heightmapResolution);
        int gridZ = int(normalizedZ/2.f * heightmap.heightmapResolution);

        //calculate ranges to loop iterate through based on radius
        int startX = gridX - radius;
        int endX = gridX + radius;
        int startZ = gridZ - radius;
        int endZ = gridZ + radius;

        //loop through ranges
        for (int z = startZ; z <= endZ; z++) {
            for (int x = startX; x <= endX; x++) {
                if (x < 0 || x >= heightmap.heightmapResolution ||
                    z < 0 || z >= heightmap.heightmapResolution) {
                    continue;
                }

                //revert to world coords
                float worldX = ((float(x) / heightmap.heightmapResolution) * 2.f - 1.f) * heightmap.heightmapSize;
                float worldZ = ((float(z) / heightmap.heightmapResolution) * 2.f - 1.f) * heightmap.heightmapSize;

                //distance between center and current point
                float distance = std::sqrt((worldX - pointX) * (worldX - pointX) + (worldZ - pointZ) * (worldZ - pointZ));

                if (distance > radius) 
                    continue;

                float intensity = 1.f;
                if (brushType == 0) {
                    intensity = Step(distance, radius, minRadius);
                }
                if (brushType == 1) {
                    intensity = LinearSmoothstep(distance, radius, minRadius);
                }
                if (brushType == 2) {
                    intensity = Polynomial(distance, radius, minRadius);
                }
                if (brushType == 3) {
                    intensity = Logarithmic(distance, radius, minRadius);
                }
                if (brushType == 4) {
                    intensity = Exponential(distance, radius, minRadius);
                }

                heightmap.map[z * heightmap.heightmapResolution + x] += strength * intensity;
                if (heightmap.map[z * heightmap.heightmapResolution + x] < heightmap.minHeight) {
                    heightmap.minHeight = heightmap.map[z * heightmap.heightmapResolution + x];
                }
                if (heightmap.map[z * heightmap.heightmapResolution + x] > heightmap.maxHeight) {
                    heightmap.maxHeight = heightmap.map[z * heightmap.heightmapResolution + x];
                }
            }
        }
    }

    static float Step(float distance, float maxRadius, float minRadius) {
        return (distance <= minRadius) ? 1.f : 0;
    }
    static float LinearSmoothstep(float distance, float maxRadius, float minRadius) {
        // as distance gets closer to maxRadius, the weaker the intensity
        float linear = glm::clamp(1.f - (distance - minRadius) / (maxRadius - minRadius), 0.f, 1.0f);
        return linear * linear * (3 - 2 * linear); //smooth interpolation
    }

    static float Polynomial(float distance, float maxRadius, float minRadius) {
        float linear = glm::clamp(1.f - (distance - minRadius) / (maxRadius - minRadius), 0.f, 1.0f);
        return glm::pow(linear, 2.f); //sharper near center
    }

    static float Logarithmic(float distance, float maxRadius, float minRadius) {
        float linear = glm::clamp(1.f - (distance - minRadius) / (maxRadius - minRadius), 0.f, 1.0f);
        return glm::log(1.0f + linear * 9.f) / glm::log(10.0f); //gradual effect 
    }

    static float Exponential(float distance, float maxRadius, float minRadius) {
        float linear = glm::clamp(1.f - (distance - minRadius) / (maxRadius - minRadius), 0.f, 1.0f);
        return glm::exp(linear * -1.5f); //ridge effect near maxRadius
    }

};