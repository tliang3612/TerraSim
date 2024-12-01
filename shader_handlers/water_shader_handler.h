#pragma once
#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include "shader_handler.h"

class WaterShaderHandler : public ShaderHandler {
public:
	WaterShaderHandler();

	GLuint uViewProjection;
	GLuint uReflectionTexture;
	GLuint uRefractionTexture;
	GLuint uDudvmap;
	GLuint uNormalmap;
	GLuint uDepthmap;
	GLuint uMoveFactor;
	GLuint uCameraPosition;
	GLuint uLightDirection;
	GLuint uBrightness;
	GLuint uSunIntensity;
	GLuint uSunFalloff;
	GLuint uWaterHeight;
	GLuint uWaterShininess;
	

	void SetViewProjection(glm::mat4 viewProjection);
	void SetMoveFactor(float moveFactor);
	void SetCameraPosition(glm::vec3 cameraPosition);
	void SetLightDirection(glm::vec3 lightDireciton);
	void SetBrightness(float brightness);
	void SetSunFalloff(float sunFalloff);
	void SetSunIntensity(float sunIntensity);
	void SetWaterHeight(float waterHeight);
	void SetWaterShininess(float waterShininess);

};