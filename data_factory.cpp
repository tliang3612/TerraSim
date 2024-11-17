#include "data_factory.h"
#include "util/util.h"
#include <string>
#include <stb/stb_image.h>

// Creates a VAO to be used for configuring multiple VBO data
GLuint DataFactory::CreateVAO(){
	GLuint vaoID;
	glGenVertexArrays(1, &vaoID);
	m_vaoList.push_back(vaoID);
	return vaoID;
}

//Creates a VBO to be used for storing vertex data
GLuint DataFactory::CreateVBO(){
	GLuint vboID;
	glGenBuffers(1, &vboID);
	m_vboList.push_back(vboID);
	return vboID;
}

//Create frame buffer
GLuint DataFactory::CreateFBO() {
	GLuint framebufferID;
	glGenFramebuffers(1, &framebufferID);
	m_fboList.push_back(framebufferID);
	return framebufferID;
}


//Create an opengl texture
GLuint DataFactory::CreateTexture(){
	GLuint textureID;
	glGenTextures(1, &textureID);
	m_textureList.push_back(textureID);
	return textureID;
}


// Create a model from model data
Model DataFactory::CreateModel(float* vertices, float* textures, int vertexCount)
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

// Create a model from model data
Model DataFactory::CreateModelWithoutTextures(float* vertices, int vertexCount)
{
	//create vao
	GLuint vaoID = CreateVAO();

	//bind the vao, making all subsequent operations configure this active vao.
	glBindVertexArray(vaoID);
	/*
	For all models created, the attribute indices are configured as such:
		0 for vertices
	*/

	//Create 2 vbos that dictate how data is layed out for this model
	CreateAndPopulateBuffer(0, 3, vertices, vertexCount);

	glBindVertexArray(0);
	return Model(vaoID, vertexCount);
}

// Store object data in the vertex buffer object.
void DataFactory::CreateAndPopulateBuffer(int attributeIndex, int elementWidth, float* data, int dataLength) {
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

GLuint DataFactory::LoadTexture(std::string texturePath){
	//stb stuff -------------------------------------------->
	stbi_set_flip_vertically_on_load(1);

	int width = 0;
	int height = 0;
	int bpp = 0;

	//load texture from file
	unsigned char* pImageData = stbi_load(texturePath.c_str(), &width, &height, &bpp, STBI_rgb_alpha);

	if (!pImageData) {
		util::fatal_error("Can't load texture from '%s' - %s\n", texturePath.c_str(), stbi_failure_reason());
	}

	printf("Loaded texture with Width: %d, height: %d, bpp: %d\n", width, height, bpp);

	GLuint textureID = CreateTexture();
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); //set to trilinear filtering for smoothness
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImageData);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(pImageData);

	return textureID;
}

GLuint DataFactory::LoadCubemapTexture(std::vector<std::string> texturePaths)
{
	GLuint textureID = CreateTexture();
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (int i = 0; i < texturePaths.size(); i++) {
		int width;
		int height;
		int channels;
		stbi_set_flip_vertically_on_load(0);
		unsigned char* data = stbi_load(texturePaths[i].c_str(), &width, &height, &channels, STBI_rgb);
		if (!data) {
			util::fatal_error("Can't load texture from '%s' - %s\n", texturePaths[i].c_str(), stbi_failure_reason());
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}



void DataFactory::DeleteDataObjects()
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
