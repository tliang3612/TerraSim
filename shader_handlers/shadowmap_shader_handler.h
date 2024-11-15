#pragma once
#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include "shader_handler.h"

class ShadowmapShaderHandler : public ShaderHandler{
public:
	ShadowmapShaderHandler();

	GLuint uLightProjection;

	void SetLightViewProjection(glm::mat4 lightProjection);

};