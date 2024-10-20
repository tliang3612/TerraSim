#include <glad/glad.h>
#include <iostream>
#include <imgui/imgui.h>
#include <SDL.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include "renderer.h"
#include "terrain.h"
#include "model_factory.h"
#include "shader_handler.h"
#include "camera.h"
#undef main // Fixes weird SDL issue


int main()
{
	const int displayWidth = 1280;
	const int displayHeight = 720;
	int width = 0;
	int height = 0;
	Renderer renderer = Renderer("TerraSim Prototype", displayWidth, displayHeight);

	//create the camera
	Camera camera = Camera(glm::vec3(0.0f, 100.0f, 200.0f), 0.0f, 0.0f, 0.0f);

	ShaderHandler shaderHandler = ShaderHandler();
	ModelFactory modelFactory = ModelFactory();

	//Terrain logic
	TerrainFactory terrainFactory = TerrainFactory();
	Terrain terrain = terrainFactory.GenerateTerrain(modelFactory, 500, 384);
	for (int i = 0; i < 100; i++) {
		std::cout << terrain.GetHeightmap().map[i] << std::endl;
	}

	bool shouldRun = true;
	while (shouldRun) {

		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			ImGui_ImplSDL2_ProcessEvent(&e);

			if (e.type == SDL_QUIT) {
				shouldRun = false;
			}

			// prepare the next frame for rendering
			renderer.PrepareFrame();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// Render the terrain.
			shaderHandler.Enable();
			shaderHandler.SetProjection(renderer.GetProjectionMatrix());
			shaderHandler.SetView(camera.GetViewMatrix());

			renderer.RenderTerrain(terrain, shaderHandler);

			shaderHandler.Disable();

			renderer.PrepareImGuiFrame();
			//render the ImGUI
			{
				ImGui::Begin("Light Settings"); {
					ImGui::PushItemWidth(150);
					static float azimuthVal = 0.f;
					static float altitudeVal = 0.f;
					static float intensity = 0.f;
					ImGui::SliderFloat("Light Azimuth", &azimuthVal, 0.0f, 360.0f);
					ImGui::SliderFloat("Light Altitude", &altitudeVal, 0.0f, 360.0f);
					ImGui::SliderFloat("Light Intensity", &intensity, 0.0f, 100.f);

					ImGui::PopItemWidth();
				}
				ImGui::End();

				ImGui::Begin("Terrain Generation Settings"); {
					ImGui::PushItemWidth(150);
					static const char* noiseTypes[] = {
						"Perlin",
						"Fractal"
					};
					static int noiseType = 0;
					ImGui::Combo("Noise Type", &noiseType, noiseTypes, sizeof(noiseTypes) / sizeof(noiseTypes[0]));

					static float amplitude = 0.1f;
					ImGui::SliderFloat("Amplitude", &amplitude, 0.0f, 1.0f);

					ImGui::Button("Generate Terrain");

					ImGui::PopItemWidth();
				}
				ImGui::End();

				ImGui::Begin("Brush Settings"); {
					ImGui::PushItemWidth(150);

					static const char* brushTypes[] = {
						"Sculpt",
						"Average"
					};
					static int brushType = 0;
					ImGui::Combo("Brush Type", &brushType, brushTypes, sizeof(brushTypes) / sizeof(brushTypes[0]));

					static float strength = 0.5f;
					ImGui::SliderFloat("Strength", &strength, 0.0f, 1.0f);

					ImGui::PopItemWidth();
				}
				ImGui::End();

				ImGui::Begin("Export"); {
					ImGui::PushItemWidth(150);

					ImGui::Button("Export to .obj");

					ImGui::PopItemWidth();
				}
				ImGui::End();

				ImGui::Begin("Texture Settings"); {
					ImGui::PushItemWidth(150);

					static char* textures[4] = { nullptr, nullptr, nullptr, nullptr };

					ImGui::Button("tex1", ImVec2(35.f, 35.f));
					ImGui::SameLine();
					ImGui::Text("Texture 1");

					ImGui::Button("tex2", ImVec2(35.f, 35.f));
					ImGui::SameLine();
					ImGui::Text("Texture 2");

					ImGui::Button("tex3", ImVec2(35.f, 35.f));
					ImGui::SameLine();
					ImGui::Text("Texture 3");

					ImGui::Button("tex4", ImVec2(35.f, 35.f));
					ImGui::SameLine();
					ImGui::Text("Texture 4");

					ImGui::PopItemWidth();
				}
				ImGui::End();

				ImGui::Begin("Debug"); {
					glm::vec3 cameraRotation = camera.GetRotation();
					ImGui::Text("Camera X: %f", camera.position.x);
					ImGui::Text("Camera Y: %f", camera.position.y);
					ImGui::Text("Camera Z: %f", camera.position.z);
					ImGui::Text("Camera Pitch: %f", glm::degrees(cameraRotation.x));
					ImGui::Text("Camera Yaw: %f", glm::degrees(cameraRotation.y));
					ImGui::Text("Camera Roll: %f", glm::degrees(cameraRotation.z));
				}
				ImGui::End();
			}

			//render the new imgui frame
			renderer.RenderImGuiFrame();
			renderer.Update();
		}
	}
	shaderHandler.Destroy();
	modelFactory.DeleteDataObjects();
	renderer.Destroy();

	return 0;
}

