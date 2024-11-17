#include "cubemap.h"

Cubemap::Cubemap(Model model, GLuint textureID) {
	m_model = model;
	m_textureID = textureID;
}



Cubemap CubemapFactory::GenerateCubemap(DataFactory dataFactory, GLuint textureID){
	float size = 500.f;

	//positions for a basic cube
	float vertices[] = {
		-size,  size, -size,
		-size, -size, -size,
		size, -size, -size,
		 size, -size, -size,
		 size,  size, -size,
		-size,  size, -size,

		-size, -size,  size,
		-size, -size, -size,
		-size,  size, -size,
		-size,  size, -size,
		-size,  size,  size,
		-size, -size,  size,

		 size, -size, -size,
		 size, -size,  size,
		 size,  size,  size,
		 size,  size,  size,
		 size,  size, -size,
		 size, -size, -size,

		-size, -size,  size,
		-size,  size,  size,
		 size,  size,  size,
		 size,  size,  size,
		 size, -size,  size,
		-size, -size,  size,

		-size,  size, -size,
		 size,  size, -size,
		 size,  size,  size,
		 size,  size,  size,
		-size,  size,  size,
		-size,  size, -size,

		-size, -size, -size,
		-size, -size,  size,
		 size, -size, -size,
		 size, -size, -size,
		-size, -size,  size,
		 size, -size,  size
	};

	Model cubemapModelData = dataFactory.CreateModelWithoutTextures(vertices, 36);
	return Cubemap(cubemapModelData, textureID);
}
