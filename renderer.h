#pragma once
#include <string>
#include "display.h"
#include "terrain.h"
#include "cubemap.h"
#include "water.h"
#include "shader_handlers/terrain_shader_handler.h"
#include "shader_handlers/skybox_shader_handler.h"
#include "shader_handlers/water_shader_handler.h"

class Renderer {

private:
	Display m_display;
	const float m_fov = 60.0f;
	const float m_nearPlane = 1.f;
	const float m_farPlane = 3000.0f;
	int m_width;
	int m_height;
	glm::mat4 m_projection;


public:
	Renderer() = default;
	Renderer(std::string title, int width, int height);

	const glm::mat4 GetProjectionMatrix() { return m_projection; }
	void PrepareFrame();
	void PrepareImGuiFrame();
	void RenderImGuiFrame();
	void RenderTerrain(const Terrain& terrain, TerrainShaderHandler terrainShaderHandler, Shadowmap shadowmap);
	void RenderSkybox(Cubemap cubemap, SkyboxShaderHandler shader);
	void RenderWater(Water water, WaterShaderHandler shader);
	void Update();
	void Destroy();
};