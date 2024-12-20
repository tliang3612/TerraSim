#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl2.h>
#include "renderer.h"



Renderer::Renderer(std::string title, int width, int height) : m_width(width), m_height(height){

	m_display = Display(title, m_width, m_height);
	SDL_GL_GetDrawableSize(m_display.GetWindow(), &m_width, &m_height);
	m_projection = glm::perspectiveFov(
		glm::radians(m_fov),
		float(m_display.GetWidth()),
		float(m_display.GetHeight()),
		m_nearPlane, 
		m_farPlane
	);
}

// Prepare ImGui.
void Renderer::PrepareImGuiFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

void Renderer::PrepareFrame(){
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	//clear color and depth buffer before rendering new frame
	glViewport(0, 0, m_width, m_height);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Render ImGui.
void Renderer::RenderImGuiFrame() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::RenderTerrain(const Terrain& terrain, TerrainShaderHandler terrainShaderHandler, Shadowmap shadowmap)
{
	glBindVertexArray(terrain.GetModel().vaoID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	terrainShaderHandler.LoadUniformSampler2D(terrainShaderHandler.uHeightmap, GL_TEXTURE0, terrain.GetHeightmap()->GetTextureID());
	terrainShaderHandler.LoadUniformSampler2D(terrainShaderHandler.uBaseTexture, GL_TEXTURE1, terrain.GetTextureIDs()[0]);
	terrainShaderHandler.LoadUniformSampler2D(terrainShaderHandler.uGroundTexture, GL_TEXTURE2, terrain.GetTextureIDs()[1]);
	terrainShaderHandler.LoadUniformSampler2D(terrainShaderHandler.uRockTexture, GL_TEXTURE3, terrain.GetTextureIDs()[2]);
	terrainShaderHandler.LoadUniformSampler2D(terrainShaderHandler.uPeaksTexture, GL_TEXTURE4, terrain.GetTextureIDs()[3]);
	terrainShaderHandler.LoadUniformSampler2D(terrainShaderHandler.uShadowmap, GL_TEXTURE5, shadowmap.textureID);

	glDrawArrays(GL_TRIANGLES, 0, terrain.GetModel().vertexCount);

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void Renderer::RenderSkybox(Cubemap cubemap, SkyboxShaderHandler skyboxShaderHandler) {
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE); 

	glBindVertexArray(cubemap.GetModel().vaoID);
	glEnableVertexAttribArray(0);
	skyboxShaderHandler.LoadUniformSamplerCube(skyboxShaderHandler.uCubemap, GL_TEXTURE0, cubemap.GetTextureID());
	glDrawArrays(GL_TRIANGLES, 0, cubemap.GetModel().vertexCount);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);

	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE); 
	glEnable(GL_DEPTH_TEST);
}

void Renderer::RenderWater(Water water, WaterShaderHandler shader){
	glBindVertexArray(water.GetModel().vaoID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	shader.LoadUniformSampler2D(shader.uReflectionTexture, GL_TEXTURE0, water.GetReflectionTextureID());
	shader.LoadUniformSampler2D(shader.uRefractionTexture, GL_TEXTURE1, water.GetRefractionTextureID());
	shader.LoadUniformSampler2D(shader.uDudvmap, GL_TEXTURE2, water.GetDudvTextureID());
	shader.LoadUniformSampler2D(shader.uNormalmap, GL_TEXTURE3, water.GetNormalmapTextureID());
	shader.LoadUniformSampler2D(shader.uDepthmap, GL_TEXTURE4, water.GetDepthmapTextureID());


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_TRIANGLES, 0, water.GetModel().vertexCount);
	glDisable(GL_BLEND);

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void Renderer::Update() {
	m_display.Update();
}

void Renderer::Destroy() {
	m_display.Destroy();
}
