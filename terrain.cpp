#include "terrain.h"
#include "data_factory.h"
#include <vector>

Terrain::Terrain(Model model, Heightmap heightmap, Shadowmap shadowmap, std::vector<GLuint> textureIDs)
{
    m_textureIDs = textureIDs;
	m_model = model;
	m_heightmap = heightmap; 
    m_shadowmap = shadowmap;
}


void Terrain::SetHeightAtPoint(float x, float y, float height){

}

const float Terrain::GetHeightFromWorld(int pointX, int pointZ){
    // terrain coords go from -heightmapSize/2 to +heightmapSize/2. normalize to [0,1]
    float normalizedX = (pointX / m_heightmap.heightmapSize + 1.0f) * 0.5f; //[0, 1]
    float normalizedZ = (pointZ / m_heightmap.heightmapSize + 1.0f) * 0.5f;

    int gridX = int(normalizedX * m_heightmap.heightmapResolution);
    int gridZ = int(normalizedZ * m_heightmap.heightmapResolution);

    if (gridX < 0 || gridX >= m_heightmap.heightmapResolution ||
        gridZ < 0 || gridZ >= m_heightmap.heightmapResolution) {
        return 0.0f; 
    }

    return m_heightmap.map[gridZ * m_heightmap.heightmapResolution + gridX];
}

void Terrain::Update(){
	m_heightmap.Update();
}

void Terrain::UpdateTexture(int index, GLuint newTextureID){
    if (index >= 0 && index < m_textureIDs.size()) {
        m_textureIDs[index] = newTextureID; // Update the texture ID in the array
    }
}

void Terrain::Destroy() {
	m_heightmap.Destroy();
}

//size is used to scale the terrain, distance between each 
Terrain TerrainFactory::GenerateTerrain(DataFactory dataFactory, float size, int resolution, std::vector<GLuint> textureIDs)
{
    std::vector<float> vertices;
    std::vector<float> textures;
    std::vector<int> indices;

    //calculate the step size for texture coordinate
    float step = 1.0f / (resolution - 1); //-1 because loop goes up to resolution-1

    //generate vertices and texture coordinates
    for (int i = 0; i < resolution; i++) {
        float u = i * step; //normalize to be between [0,1]
        float x = u * size * 2.0f - size;

        for (int j = 0; j < resolution; j++) {
            float v = j * step;
            float z = v * size * 2.0f - size;

            //store vertex position
            vertices.push_back(x);  
            vertices.push_back(0.0f);  
            vertices.push_back(z);  

            //store texture coordinates
            textures.push_back(u); 
            textures.push_back(v); 
        }
    }

    //generate indices for quads (two triangles per quad)
    for (int z = 0; z < resolution - 1; z++) {
        for (int x = 0; x < resolution - 1; x++) {
            int topLeft = z * resolution + x; 
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * resolution + x;
            int bottomRight = bottomLeft + 1;


            //first triangle (topleft)
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            //second triangle (bottom right)
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);

        }
    }

    //reorder vertices and textures based on indices. Need to format it like such for VBO:
    //[x,y,z,u,v] <- vertex buffer data layout for each index
    std::vector<float> verticesOut, texturesOut;
    for (const int index : indices) {
        //grav each vertex and texture data based on the index
        verticesOut.push_back(vertices[index * 3 + 0]); //x
        verticesOut.push_back(vertices[index * 3 + 1]); //y
        verticesOut.push_back(vertices[index * 3 + 2]); //z
        texturesOut.push_back(textures[index * 2 + 0]); //u
        texturesOut.push_back(textures[index * 2 + 1]); //v
    }

    //create model with vertex and texture data
    Model terrainModelData = dataFactory.CreateModel(verticesOut.data(), texturesOut.data(), verticesOut.size() / 3);
    Model shadowmapModelData = dataFactory.CreateModelWithoutTextures(verticesOut.data(), verticesOut.size() / 3);
    return Terrain(terrainModelData, Heightmap(size, resolution, dataFactory), Shadowmap(resolution*2, dataFactory), textureIDs);
}
