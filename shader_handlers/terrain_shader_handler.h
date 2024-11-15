#pragma once
#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include "shader_handler.h"

class TerrainShaderHandler: public ShaderHandler {
public:
	TerrainShaderHandler();

	GLuint uViewProjection;
	GLuint uClip;
	GLuint uLightDirection;
	GLuint uHeightmap;
	GLuint uMinHeight;
	GLuint uMaxHeight;
	GLuint uIndicatorPosition;
	GLuint uIndicatorRadius;
	GLuint uBaseTexture;
	GLuint uGroundTexture;
	GLuint uRockTexture;
	GLuint uPeaksTexture;
	GLuint uShadowmap;
	GLuint uLightViewProjection;
	GLuint uCameraPosition;
	GLuint uLightIntensity;
	GLuint uTextureScale;

	void SetClip(glm::vec4 clip);
	void SetLightDirection(glm::vec3 lightDirection);
	void SetViewProjection(glm::mat4 viewProjection);
	void SetMinHeight(float minHeight);
	void SetMaxHeight(float maxHeight);
	void SetIndicatorPosition(glm::vec2 indicatorPosition);
	void SetIndicatorRadius(float indicatorRadius);
	void SetLightViewProjection(glm::mat4 lightViewProjection);
	void SetCameraPosition(glm::vec3 cameraPosition);
	void SetLightIntensity(float lightIntensity);
	void SetTextureScale(float textureScale);
};