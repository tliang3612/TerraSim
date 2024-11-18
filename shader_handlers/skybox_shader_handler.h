#pragma once
#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include "shader_handler.h"

class SkyboxShaderHandler : public ShaderHandler {
public:
	SkyboxShaderHandler();

	GLuint uViewProjection;
	GLuint uCubemap;
	GLuint uLightDirection;
	GLuint uSunFalloff;
	GLuint uSunIntensity;

	void SetViewProjection(glm::mat4 viewProjection);
	void SetLightDirection(glm::vec3 lightDirection);
	void SetSunFalloff(float sunFalloff);
	void SetSunIntensity(float sunIntensity);
};