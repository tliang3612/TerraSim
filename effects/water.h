#pragma once
#include "../engine/data_factory.h"

class Water {
public:
	Water(Model model, DataFactory dataFactory, GLuint dudvMap, GLuint normalmap, float size, int displayWidth, int displayHeight);
	Water() = default;

	const Model& GetModel() const { return m_model; }
	const GLuint GetReflectionTextureID() const { return m_reflectionTextureID; }
	const GLuint GetRefractionTextureID() const { return m_refractionTextureID; }
	const GLuint GetDudvTextureID() const { return m_dudvMapTextureID; }
	const GLuint GetNormalmapTextureID() const { return m_normalmapTextureID; }
	const GLuint GetDepthmapTextureID() const { return m_refractionDepthTextureID; }
	void SetWaterShiniess(float shininess){}
	void BindFramebuffer(int frameBufferType);
	void UnbindFramebuffer();
	GLuint CreateTextureAttachment(DataFactory dataFactory, int width, int height);
	GLuint CreateDepthTextureAttachment(DataFactory dataFactory, int width, int height);

	float WaterHeight = 0.f;
	float WaveSpeed = 0.006f;
	float WaterShininess = 1.f;

private:
	Model m_model;
	GLuint m_dudvMapTextureID;
	GLuint m_normalmapTextureID;

	float m_size;
	int m_width;
	int m_height;

	GLuint m_reflectionFboID;
	GLuint m_reflectionTextureID;
	GLuint m_reflectionDepthTextureID;

	GLuint m_refractionFboID;
	GLuint m_refractionTextureID;
	GLuint m_refractionDepthTextureID;
};

class WaterFactory {
public:
	WaterFactory() = default;

	Water GenerateWater(DataFactory dataFactory, GLuint dudvmap, GLuint normalmap, float size, int displayWidth, int displayHeight);

};