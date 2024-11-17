#pragma once
#include <glad/glad.h>
#include <vector>
#include <string>

struct Model {
	GLuint vaoID;
	int vertexCount;
	
	Model() = default;

	Model(GLuint vaoID, int vertexCount) {
		this->vaoID = vaoID;
		this->vertexCount = vertexCount;
	}
};



class DataFactory {
public:
	GLuint CreateVAO();
	GLuint CreateVBO();
	GLuint CreateTexture();
	GLuint CreateFBO();
	void CreateAndPopulateBuffer(int attributeIndex, int elementWidth, float* data, int dataLength);
	Model CreateModel(float* vertices, float* textures, int vertexCount);
	Model CreateModelWithoutTextures(float* vertices, int vertexCount);
	GLuint LoadTexture(std::string texturePath);
	GLuint LoadCubemapTexture(std::vector<std::string> texturePaths);
	void DeleteDataObjects();

private: 
	std::vector<GLuint> m_vaoList;
	std::vector<GLuint> m_vboList;
	std::vector<GLuint> m_fboList;
	std::vector<GLuint> m_textureList;

};





