#include "renderer.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl2.h>
#include <glad/glad.h>
#include "terrain.h"
#include "shader_handler.h"


Renderer::Renderer(std::string title, int width, int height)
{
	m_display = Display(title, width, height);
	SDL_GL_GetDrawableSize(m_display.GetWindow(), &m_width, &m_height);
	m_projection = glm::perspectiveFov(
		glm::radians(FOV),
		float(m_display.GetWidth()),
		float(m_display.GetHeight()),
		NEAR_PLANE, 
		FAR_PLANE
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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// Render ImGui.
void Renderer::RenderImGuiFrame() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::RenderTerrain(Terrain terrain, ShaderHandler shaderHandler)
{
	glBindVertexArray(terrain.GetModel().vaoID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	shaderHandler.SetUniformSampler2D(shaderHandler.uHeightmap, GL_TEXTURE0, terrain.GetHeightmap().textureID);
	shaderHandler.SetUniformSampler2D(shaderHandler.uTexture1, GL_TEXTURE1, terrain.GetTextureIDs()[0]);
	shaderHandler.SetUniformSampler2D(shaderHandler.uTexture2, GL_TEXTURE2, terrain.GetTextureIDs()[1]);
	shaderHandler.SetUniformSampler2D(shaderHandler.uTexture3, GL_TEXTURE3, terrain.GetTextureIDs()[2]);
	shaderHandler.SetUniformSampler2D(shaderHandler.uTexture3, GL_TEXTURE3, terrain.GetTextureIDs()[3]);

	glDrawArrays(GL_TRIANGLES, 0, terrain.GetModel().vertexCount);

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
