#pragma once
#include <string>
#include "display.h"
#include "terrain.h"
#include "shader_handler.h"

class Renderer {

private:
	Display m_display;
	const float FOV = 60.0f;
	const float NEAR_PLANE = 0.5f;
	const float FAR_PLANE = 5000.0f;
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
	void RenderTerrain(Terrain terrain, ShaderHandler shaderHandler);
	void Update();
	void Destroy();
};