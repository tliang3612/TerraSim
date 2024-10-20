#pragma once

#include <glad/glad.h>
#include <vector>

struct Model {
	GLuint vaoID;
	int vertexCount;
	
	Model() = default;

	Model(GLuint vaoID, int vertexCount) {
		this->vaoID = vaoID;
		this->vertexCount = vertexCount;
	}
};

class ModelFactory {
public:
	GLuint CreateVAO();
	GLuint CreateVBO();
	GLuint CreateTexture();
	void CreateAndPopulateBuffer(int attributeIndex, int elementWidth, float* data, int dataLength);
	Model CreateModel(float* vertices, float* textures, int vertexCount);
	void DeleteDataObjects();

private: 
	std::vector<GLuint> m_vaoList;
	std::vector<GLuint> m_vboList;
	std::vector<GLuint> m_textureList;
	std::vector<GLuint> m_frameBufferList;

};



