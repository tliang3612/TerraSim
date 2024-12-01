#include "../util/util.h"
#include "shadowmap_shader_handler.h"

const std::string VERTEX_SHADER = "shaders/shadowmap.vs";
const std::string FRAGMENT_SHADER = "shaders/shadowmap.fs";

ShadowmapShaderHandler::ShadowmapShaderHandler() {
    LoadShaders(VERTEX_SHADER, FRAGMENT_SHADER);
    uLightProjection = GetUniformLocation("uLightProjection");
    BindAttribute(0, "iPosition");
}

void ShadowmapShaderHandler::SetLightViewProjection(glm::mat4 lightViewProjection) {
    LoadUniformMatrix4(uLightProjection, lightViewProjection);
}