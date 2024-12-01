#include "../util/util.h"
#include "water_shader_handler.h"

const std::string VERTEX_SHADER = "shaders/water.vs";
const std::string FRAGMENT_SHADER = "shaders/water.fs";

WaterShaderHandler::WaterShaderHandler() {
    LoadShaders(VERTEX_SHADER, FRAGMENT_SHADER);
    uViewProjection = GetUniformLocation("uViewProjection");
    uRefractionTexture = GetUniformLocation("uRefractionTexture");
    uReflectionTexture = GetUniformLocation("uReflectionTexture");
    uDudvmap = GetUniformLocation("uDudvmap");
    uNormalmap = GetUniformLocation("uNormalmap");
    uMoveFactor = GetUniformLocation("uMoveFactor");
    uCameraPosition = GetUniformLocation("uCameraPosition");
    uLightDirection = GetUniformLocation("uLightDirection");
    uDepthmap = GetUniformLocation("uDepthmap");
    uBrightness = GetUniformLocation("uBrightness");
    uSunIntensity = GetUniformLocation("uSunIntensity");
    uSunFalloff = GetUniformLocation("uSunFalloff");
    uWaterShininess = GetUniformLocation("uWaterShininess");
    uWaterHeight = GetUniformLocation("uWaterHeight");
    BindAttribute(0, "iPosition");
    BindAttribute(1, "iTextureCoords");
}

void WaterShaderHandler::SetViewProjection(glm::mat4 viewProjection) {
    LoadUniformMatrix4(uViewProjection, viewProjection);
}

void WaterShaderHandler::SetMoveFactor(float moveFactor){
    LoadUniformFloat(uMoveFactor, moveFactor);
}

void WaterShaderHandler::SetCameraPosition(glm::vec3 cameraPosition){
    LoadUniformVec3(uCameraPosition, cameraPosition);
}

void WaterShaderHandler::SetLightDirection(glm::vec3 lightDireciton) {
    LoadUniformVec3(uLightDirection, lightDireciton);
}

void WaterShaderHandler::SetBrightness(float brightness){
    LoadUniformFloat(uBrightness, brightness);
}

void WaterShaderHandler::SetSunFalloff(float sunFalloff) {
    LoadUniformFloat(uSunFalloff, sunFalloff);
}

void WaterShaderHandler::SetSunIntensity(float sunIntensity) {
    LoadUniformFloat(uSunIntensity, sunIntensity);
}

void WaterShaderHandler::SetWaterHeight(float waterHeight){
    LoadUniformFloat(uWaterHeight, waterHeight);
}

void WaterShaderHandler::SetWaterShininess(float waterShininess){
    LoadUniformFloat(uWaterShininess, waterShininess);
}
