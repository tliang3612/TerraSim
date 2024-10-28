#include "model_factory.h"

// Creates a VAO to be used for configuring multiple VBO data
GLuint ModelFactory::CreateVAO(){
	GLuint vaoID;
	glGenVertexArrays(1, &vaoID);
	m_vaoList.push_back(vaoID);
	return vaoID;
}

//Creates a VBO to be used for storing vertex data
GLuint ModelFactory::CreateVBO(){
	GLuint vboID;
	glGenBuffers(1, &vboID);
	m_vboList.push_back(vboID);
	return vboID;
}

//Create an opengl texture
GLuint ModelFactory::CreateTexture(){
	GLuint textureID;
	glGenTextures(1, &textureID);
	m_textureList.push_back(textureID);
	return textureID;
}

// Create a model from model data
Model ModelFactory::CreateModel(float* vertices, float* textures, int vertexCount)
{
	//create vao
	GLuint vaoID = CreateVAO();

	//bind the vao, making all subsequent operations configure this active vao.
	glBindVertexArray(vaoID);
	/*
	For all models created, the attribute indices are configured as such:
		0 for vertices
		1 for textures
	*/

	//Create 2 vbos that dictate how data is layed out for this model
	CreateAndPopulateBuffer(0, 3, vertices, vertexCount);
	CreateAndPopulateBuffer(1, 2, textures, vertexCount);

	glBindVertexArray(0);
	return Model(vaoID, vertexCount);
}


// Store object data in the vertex buffer object.
void ModelFactory::CreateAndPopulateBuffer(int attributeIndex, int elementWidth, float* data, int dataLength) {
	//create vbo
	GLuint vboID = CreateVBO(); 

	//bind the vbo, making it the active buffer for storing vertex attribute data
	glBindBuffer(GL_ARRAY_BUFFER, vboID);

	//uploads vertex data of size (dataLength * elementWidth * sizeof(float)) to the GPU
	glBufferData(GL_ARRAY_BUFFER, dataLength * elementWidth * sizeof(float), data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(attributeIndex);  // Enable texture coordinate attribute

	//specify how the data should be interpreted for the given attribute index. no stride needed as each data is within its own vbo
	glVertexAttribPointer(attributeIndex, elementWidth, GL_FLOAT, GL_FALSE, 0, 0); 

	//unbind the buffer
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}



void ModelFactory::DeleteDataObjects()
{
	for (auto vao : m_vaoList) {
		glDeleteVertexArrays(1, &vao);
	}
	for (auto vbo : m_vboList) {
		glDeleteBuffers(1, &vbo);
	}
	for (auto texture : m_textureList) {
		glDeleteTextures(1, &texture);
	}
}
