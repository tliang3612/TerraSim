#pragma once
#include <glad/glad.h>
#include <vector>

class TextureHandler{
public:
	GLuint CreateTexture();


private:
	std::vector<GLuint> m_textureList;


};