#pragma once
#include "../engine/data_factory.h"

class Cubemap {
public:
	Cubemap(Model model, GLuint textureID);
	Cubemap() = default;

	Model &GetModel() { return m_model; }
	GLuint GetTextureID() { return m_textureID; }

private:
	Model m_model;
	GLuint m_textureID;

};

class CubemapFactory {
public:
	CubemapFactory() = default;

	Cubemap GenerateCubemap(DataFactory dataFactory, GLuint textureID);

};