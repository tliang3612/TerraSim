#include "terrain.h"

Terrain::Terrain(Model model, std::shared_ptr<Heightmap> heightmap, Shadowmap shadowmap, std::vector<GLuint> textureIDs, std::vector<float> textureCoords, std::vector<float> vertices, std::vector<int> indices)
    : m_textureIDs(textureIDs), m_model(model), m_shadowmap(shadowmap), m_heightmap(heightmap),
      m_textureCoords(textureCoords), m_vertices(vertices), m_indices(indices){

}


const float Terrain::GetHeightFromWorld(int pointX, int pointZ) const {

    float heightmapSize = m_heightmap->GetSize();
    int heightmapResolution = m_heightmap->GetResolution();

    // terrain coords go from -heightmapSize/2 to +heightmapSize/2. normalize to [0,1]
    float normalizedX = (pointX / heightmapSize + 1.f) * 0.5f; //[0, 1]
    float normalizedZ = (pointZ / heightmapSize + 1.f) * 0.5f;

    int gridX = int(normalizedX * heightmapResolution);
    int gridZ = int(normalizedZ * heightmapResolution);

    return m_heightmap->GetHeight(gridX, gridZ);
}

void Terrain::Update(){
	m_heightmap->Update();
}

void Terrain::UpdateTexture(int index, GLuint newTextureID){
    if (index >= 0 && index < m_textureIDs.size()) {
        m_textureIDs[index] = newTextureID; // Update the texture ID in the array
    }
}

void Terrain::UpdateSize(float size){
    
}

//size is used to scale the terrain, distance between each 
Terrain TerrainFactory::GenerateTerrain(DataFactory dataFactory, float size, int resolution, std::vector<GLuint> textureIDs, float noiseSeed){
    std::vector<float> vertices;
    std::vector<float> textureCoords;
    std::vector<int> indices;

    //calculate the step size for texture coordinate
    float step = 1.f / (resolution - 1); //-1 because loop goes up to resolution-1

    //generate vertices and texture coordinates
    for (int i = 0; i < resolution; i++) {
        float u = i * step; //normalize to be between [0,1]
        float x = u * size * 2.f - size;

        for (int j = 0; j < resolution; j++) {
            float v = j * step;
            float z = v * size * 2.f - size;

            //store vertex position
            vertices.push_back(x);  
            vertices.push_back(0.f);  
            vertices.push_back(z);  

            //store texture coordinates
            textureCoords.push_back(u); 
            textureCoords.push_back(v); 

            //insert here
            if (i < resolution - 1 && j < resolution - 1) {
                int topLeft = i * resolution + j;
                int topRight = topLeft + 1;
                int bottomLeft = (i + 1) * resolution + j;
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
    }

    //reorder vertices and textures based on indices. Need to format it like such for VBO:
    //[x,y,z,u,v] <- vertex buffer data layout for each index
    std::vector<float> verticesOut;
    std::vector<float> texturesOut;
    for (int index : indices) {
        //grav each vertex and texture data based on the index
        verticesOut.push_back(vertices[index * 3 + 0]); //x
        verticesOut.push_back(vertices[index * 3 + 1]); //y
        verticesOut.push_back(vertices[index * 3 + 2]); //z
        texturesOut.push_back(textureCoords[index * 2 + 0]); //u
        texturesOut.push_back(textureCoords[index * 2 + 1]); //v
    }

    //create model with vertex and texture data
    Model terrainModelData = dataFactory.CreateModel(verticesOut.data(), texturesOut.data(), verticesOut.size() / 3);
    Model shadowmapModelData = dataFactory.CreateModelWithoutTextureCoords(verticesOut.data(), verticesOut.size() / 3);
    std::shared_ptr<Heightmap> heightmap = std::make_shared<Heightmap>(size, resolution, noiseSeed, dataFactory);
    Shadowmap shadowmap = Shadowmap(resolution, dataFactory);
    return Terrain(terrainModelData, heightmap, shadowmap, textureIDs, textureCoords, vertices, indices);
}
