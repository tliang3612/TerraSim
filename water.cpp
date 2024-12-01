#include "water.h"
#include "data_factory.h"

Water::Water(Model model, DataFactory dataFactory, GLuint dudvMapTextureID, GLuint normalmap, float size, int displayWidth, int displayHeight)
	: m_model(model), m_dudvMapTextureID(dudvMapTextureID), m_normalmapTextureID(normalmap), m_size(size), m_width(displayWidth), m_height(displayHeight) {

	//init the reflection and refraction fbos and textures
	int scaledWidth = m_width / m_scale;
	int scaledHeight = m_height / m_scale;

	m_reflectionFboID = dataFactory.CreateFBO();
	glBindFramebuffer(GL_FRAMEBUFFER, m_reflectionFboID);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	m_reflectionTextureID = CreateTextureAttachment(dataFactory, scaledWidth, scaledHeight);
	m_reflectionDepthTextureID = CreateDepthTextureAttachment(dataFactory, scaledWidth, scaledHeight);
	UnbindFramebuffer();

	m_refractionFboID = dataFactory.CreateFBO();
	glBindFramebuffer(GL_FRAMEBUFFER, m_refractionFboID);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	m_refractionTextureID = CreateTextureAttachment(dataFactory, scaledWidth, scaledHeight);
	m_refractionDepthTextureID = CreateDepthTextureAttachment(dataFactory, scaledWidth, scaledHeight);
	UnbindFramebuffer();
	
}

// 0 for reflection, 1 for refraction
void Water::BindFramebuffer(int frameBufferType) {
	int scaledWidth = m_width / m_scale;
	int scaledHeight = m_height / m_scale;

	GLuint fboID = frameBufferType == 0 ? m_reflectionFboID : m_refractionFboID;
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	glViewport(0, 0, scaledWidth, scaledHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CLIP_DISTANCE0);
}

void Water::UnbindFramebuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, m_width, m_height);
	glDisable(GL_CLIP_DISTANCE0);
}

GLuint Water::CreateTextureAttachment(DataFactory dataFactory, int width, int height){
	GLuint textureID = dataFactory.CreateTexture();
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);
	return textureID;
}

GLuint Water::CreateDepthTextureAttachment(DataFactory dataFactory, int width, int height){
	GLuint textureID = dataFactory.CreateTexture();
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureID, 0);
	return textureID;
}


Water WaterFactory::GenerateWater(DataFactory dataFactory, GLuint dudvMapTextureID, GLuint normalmapTextureID, float size, int displayWidth, int displayHeight) {

	//positions for a basic water quad
	float vertices[] = {
        -size, 0.f, -size,
        size, 0.f, -size,
        -size, 0.f, size,
        size, 0.f, -size,
        size, 0.f, size,
        -size, 0.f, size
	};

	float textures[] = {
		0.0f, 0.0f, 
		1.0f, 0.0f,  
		0.0f, 1.0f,  
		1.0f, 0.0f,  
		1.0f, 1.0f,  
		0.0f, 1.0f  
	};

	Model waterModelData = dataFactory.CreateModel(vertices, textures, 6);
	return Water(waterModelData, dataFactory, dudvMapTextureID, normalmapTextureID, size, displayWidth, displayHeight);
}
