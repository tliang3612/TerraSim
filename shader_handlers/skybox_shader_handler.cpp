#include "../util/util.h"
#include "skybox_shader_handler.h"

const std::string VERTEX_SHADER = "shaders/skybox.vs";
const std::string FRAGMENT_SHADER = "shaders/skybox.fs";

SkyboxShaderHandler::SkyboxShaderHandler() {
    LoadShaders(VERTEX_SHADER, FRAGMENT_SHADER);
    uViewProjection = GetUniformLocation("uViewProjection");
    uCubemap = GetUniformLocation("uCubemap");
    uLightDirection = GetUniformLocation("uLightDirection");
    uSunFalloff = GetUniformLocation("uSunFalloff");
    uSunIntensity = GetUniformLocation("uSunIntensity");
    BindAttribute(0, "iPosition");
}

void SkyboxShaderHandler::SetViewProjection(glm::mat4 viewProjection) {
    LoadUniformMatrix4(uViewProjection, viewProjection);
}

void SkyboxShaderHandler::SetLightDirection(glm::vec3 lightDirection) {
    LoadUniformVec3(uLightDirection, lightDirection);
}

void SkyboxShaderHandler::SetSunFalloff(float sunFalloff) {
    LoadUniformFloat(uSunFalloff, sunFalloff);
}

void SkyboxShaderHandler::SetSunIntensity(float sunIntensity) {
    LoadUniformFloat(uSunIntensity, sunIntensity);
}

