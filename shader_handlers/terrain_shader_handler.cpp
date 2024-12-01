#include "../util/util.h"
#include "terrain_shader_handler.h"

const std::string VERTEX_SHADER = "shaders/terrain.vs";
const std::string FRAGMENT_SHADER = "shaders/terrain.fs";

TerrainShaderHandler::TerrainShaderHandler() {
    LoadShaders(VERTEX_SHADER, FRAGMENT_SHADER);
    uClip = GetUniformLocation("uClip");
    uLightDirection = GetUniformLocation("uLightDirection");
    uViewProjection = GetUniformLocation("uViewProjection");
    uMinHeight = GetUniformLocation("uMinHeight");
    uMaxHeight = GetUniformLocation("uMaxHeight");
    uIndicatorPosition = GetUniformLocation("uIndicatorPosition");
    uIndicatorRadius = GetUniformLocation("uIndicatorRadius");
    uBaseTexture = GetUniformLocation("uBaseTexture");
    uGroundTexture = GetUniformLocation("uGroundTexture");
    uRockTexture = GetUniformLocation("uRockTexture");
    uPeaksTexture = GetUniformLocation("uPeaksTexture");
    uHeightmap = GetUniformLocation("uHeightmap");
    uShadowmap = GetUniformLocation("uShadowmap");
    uSunFalloff = GetUniformLocation("uSunFalloff");
    uSunIntensity = GetUniformLocation("uSunIntensity");
    uLightViewProjection = GetUniformLocation("uLightViewProjection");
    uCameraPosition = GetUniformLocation("uCameraPosition");
    uBrightness = GetUniformLocation("uBrightness");
    uTextureScale = GetUniformLocation("uTextureScale");
    BindAttribute(0, "iPosition");
    BindAttribute(1, "iTextureCoords");
}

void TerrainShaderHandler::SetClip(glm::vec4 clip) {
    LoadUniformVec4(uClip, clip);
}

void TerrainShaderHandler::SetLightDirection(glm::vec3 lightDirection) {
    LoadUniformVec3(uLightDirection, lightDirection);
}

void TerrainShaderHandler::SetViewProjection(glm::mat4 viewProjection) {
    LoadUniformMatrix4(uViewProjection, viewProjection);
}

void TerrainShaderHandler::SetMinHeight(float minHeight) {
    LoadUniformFloat(uMinHeight, minHeight);
}

void TerrainShaderHandler::SetMaxHeight(float maxHeight) {
    LoadUniformFloat(uMaxHeight, maxHeight);
}

void TerrainShaderHandler::SetIndicatorPosition(glm::vec2 indicatorPosition) {
    LoadUniformVec2(uIndicatorPosition, indicatorPosition);
}

void TerrainShaderHandler::SetIndicatorRadius(float indicatorRadius) {
    LoadUniformFloat(uIndicatorRadius, indicatorRadius);
}
void TerrainShaderHandler::SetLightViewProjection(glm::mat4 lightViewProjection) {
    LoadUniformMatrix4(uLightViewProjection, lightViewProjection);
}

void TerrainShaderHandler::SetBrightness(float brightness) {
    LoadUniformFloat(uBrightness, brightness);
}

void TerrainShaderHandler::SetTextureScale(float textureScale){
    LoadUniformFloat(uTextureScale, textureScale);
}

void TerrainShaderHandler::SetSunFalloff(float sunFalloff){
    LoadUniformFloat(uSunFalloff, sunFalloff);
}

void TerrainShaderHandler::SetSunIntensity(float sunIntensity){
    LoadUniformFloat(uSunIntensity, sunIntensity);
}

void TerrainShaderHandler::SetCameraPosition(glm::vec3 cameraPosition){
    LoadUniformVec3(uCameraPosition, cameraPosition);
}
