#include "texture_handler.h"

//Create an opengl texture
GLuint TextureHandler::CreateTexture() {
	GLuint textureID;
	glGenTextures(1, &textureID);
	m_textureList.push_back(textureID);
	return textureID;
}