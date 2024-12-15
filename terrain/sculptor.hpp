#include "terrain.h"

struct Sculptor {

    static void Sculpt(std::shared_ptr<Heightmap> heightmap, float pointX, float pointZ, float radius, float strength, int brushType) {
        float minRadius = radius/3;
        float heightmapSize = heightmap->GetSize();
        int heightmapResolution = heightmap->GetResolution();

        float normalizedX = (pointX / heightmapSize + 1.f); //[0, 1]
        float normalizedZ = (pointZ / heightmapSize + 1.f);

        int gridX = int(normalizedX/2.f * heightmapResolution);
        int gridZ = int(normalizedZ/2.f * heightmapResolution);

        //calculate ranges to loop iterate through based on radius
        int startX = gridX - radius;
        int endX = gridX + radius;
        int startZ = gridZ - radius;
        int endZ = gridZ + radius;

        //loop through ranges
        for (int z = startZ; z <= endZ; z++) {
            for (int x = startX; x <= endX; x++) {
                if (x < 0 || x >= heightmapResolution ||
                    z < 0 || z >= heightmapResolution) {
                    continue;
                }

                //revert to world coords
                float worldX = ((float(x) / heightmapResolution) * 2.f - 1.f) * heightmapSize;
                float worldZ = ((float(z) / heightmapResolution) * 2.f - 1.f) * heightmapSize;

                //distance between center and current point
                float distance = std::sqrt((worldX - pointX) * (worldX - pointX) + (worldZ - pointZ) * (worldZ - pointZ));

                if (distance > radius) 
                    continue;

                float intensity = 1.f;
                if (brushType == 0) {
                    intensity = Step(distance, radius, minRadius);
                }
                if (brushType == 1) {
                    intensity = LinearFalloff(distance, radius, minRadius);
                }
                if (brushType == 2) {
                    intensity = LinearSmoothstep(distance, radius, minRadius);
                }
                if (brushType == 3) {
                    intensity = Polynomial(distance, radius, minRadius);
                }
                if (brushType == 4) {
                    intensity = Logarithmic(distance, radius, minRadius);
                }

                heightmap->SetHeight(x, z, heightmap->GetHeight(x, z) + strength * intensity);
            }
        }
    }

    static float Step(float distance, float maxRadius, float minRadius) {
        return (distance <= maxRadius) ? 1.f : 0;
    }

    static float LinearFalloff(float distance, float maxRadius, float minRadius) {
        return glm::clamp(1.f - (distance - minRadius) / (maxRadius - minRadius), 0.f, 1.f);
    }

    static float LinearSmoothstep(float distance, float maxRadius, float minRadius) {
        // as distance gets closer to maxRadius, the weaker the intensity
        float linear = glm::clamp(1.f - (distance - minRadius) / (maxRadius - minRadius), 0.f, 1.f);
        return linear * linear * (3 - 2 * linear); //smooth interpolation
    }

    static float Polynomial(float distance, float maxRadius, float minRadius) {
        float linear = glm::clamp(1.f - (distance - minRadius) / (maxRadius - minRadius), 0.f, 1.f);
        return glm::pow(linear, 2.5f); //sharper near center
    }

    static float Logarithmic(float distance, float maxRadius, float minRadius) {
        float linear = glm::clamp(1.f - (distance - minRadius) / (maxRadius - minRadius), 0.f, 1.f);
        return glm::log(1.f + glm::pow(linear, .75f) * 9.f) / glm::log(10.f); //gradual effect 
    }
};