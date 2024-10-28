#include <glad/glad.h>
#include <iostream>
#include <imgui/imgui.h>
#include <SDL.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <chrono>
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
	Camera camera = Camera(glm::vec3(0.0f, 300.f, 0.0f), 0.0f, 0.f, 0.0f);

	//user inputs to move camera
	bool keyW = false, keyA = false, keyS = false, keyD = false, keyZ = false, keyX = false;
	bool keyLeftShift = false, keyRightShift = false;
	float mouseDragX = 0.f, mouseDragY = displayHeight;
	int mouseX = 0.f, mouseY = 0.f;

	//lightDirection vector
	glm::vec3 lightDirection(0.f, 1.f, 0.f);

	ShaderHandler shaderHandler = ShaderHandler();
	ModelFactory modelFactory = ModelFactory();

	//Terrain logic
	TerrainFactory terrainFactory = TerrainFactory();
	Terrain terrain = terrainFactory.GenerateTerrain(modelFactory, 500, 1000);

	float deltaTime = .1f;
	auto deltaTimeCounter = SDL_GetPerformanceCounter(); //record the deltaTime counter
	auto fpsCounter = SDL_GetPerformanceCounter(); //record the fps counter
	auto frequency = SDL_GetPerformanceFrequency(); //performance counter ticks per second
	auto frameCount = 0;
	float fps = 0;
	float targetFrameTime = 1000.f / 265.f; //cap frame rate at 265. This is to ensure that extra resources aren't wasted on this application. Might Change

	bool shouldRun = true;

	while (shouldRun) {
		
		//used to calculate delta time
		auto start = SDL_GetPerformanceCounter();

		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			ImGui_ImplSDL2_ProcessEvent(&e);

			if (e.type == SDL_QUIT) {
				shouldRun = false;
			}
			else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
				bool keyDown = (e.type == SDL_KEYDOWN);
				SDL_Keycode key = e.key.keysym.sym;

				if (key == SDLK_w) keyW = keyDown;
				if (key == SDLK_a) keyA = keyDown;
				if (key == SDLK_s) keyS = keyDown;
				if (key == SDLK_d) keyD = keyDown;
				if (key == SDLK_z) keyZ = keyDown;
				if (key == SDLK_x) keyX = keyDown;
				if (key == SDLK_LSHIFT) keyLeftShift = keyDown;
			}
			else if (e.type == SDL_MOUSEMOTION) {
				mouseX = e.motion.x;
				mouseY = e.motion.y;
				if (e.motion.state & SDL_BUTTON(SDL_BUTTON_MIDDLE)) {
					mouseDragX += e.motion.xrel;
					mouseDragY += e.motion.yrel;
				}
			}
		}

		camera.Update(deltaTime, keyW, keyA, keyS, keyD, keyZ, keyX, keyLeftShift, mouseDragX, mouseDragY, displayWidth, displayHeight);

		//FPS and DeltaTime Logic 
		{
			auto currentCounter = SDL_GetPerformanceCounter(); //ticks as of now
			float deltaTime = static_cast<float>(currentCounter - deltaTimeCounter) / frequency; //difference between start and current ticks. Divide by frequency to get seconds
			deltaTimeCounter = currentCounter; //set start ticks to current ticks

			//frames are moving too fast. pause until desired time 
			if (deltaTime < targetFrameTime) {
				SDL_Delay(static_cast<Uint32>(targetFrameTime - deltaTime));
			}

			float fpsUpdateDeltaTime = static_cast<float>(currentCounter - fpsCounter) / frequency; //delta time for fps calculations. Needs to be different var because fpsCounter 
																									//will only update every 0.1s rather than each frame
			frameCount++; //increment frames
			if (fpsUpdateDeltaTime > 0.1f) { //update fps every .1s
				fps = frameCount / fpsUpdateDeltaTime;
				fpsCounter = currentCounter;
				frameCount = 0;
			}
		}

		// prepare the next frame for rendering
		renderer.PrepareFrame();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//Terrain Render Logic
		{
			glm::vec4 clip = glm::vec4(0.0f); //no clipping as of now. might change for shadows
			shaderHandler.Enable();
			shaderHandler.SetViewProjection(renderer.GetProjectionMatrix() * camera.GetViewMatrix());
			shaderHandler.SetLightDirection(lightDirection);
			shaderHandler.SetClip(clip);

			renderer.RenderTerrain(terrain, shaderHandler);

			shaderHandler.Disable();
		}

		//render the ImGUI
		renderer.PrepareImGuiFrame();
		{
			ImGui::Begin("Light Settings"); {
				ImGui::PushItemWidth(150);
				static float azimuthVal = 0.f; //horizontal rotation of light
				static float altitudeVal = 0.f; //vertical rotation of light
				static float intensity = 0.f;
				ImGui::SliderFloat("Light Azimuth", &azimuthVal, 0.0f, 360.f);
				ImGui::SliderFloat("Light Altitude", &altitudeVal, 0.0f, 360.f);
				ImGui::SliderFloat("Light Intensity", &intensity, 0.0f, 100.f); //TODO

				float horizontalScaling = std::cos(glm::radians(altitudeVal));
				float xAngle = std::cos(glm::radians(azimuthVal)) * horizontalScaling;
				float yAngle = std::sin(glm::radians(altitudeVal));
				float zAngle = std::sin(glm::radians(azimuthVal)) * horizontalScaling;

				lightDirection = glm::normalize(glm::vec3(xAngle, yAngle, zAngle));

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

				ImGui::Text("FPS: %f", fps);
				ImGui::Text("Camera X: %f", camera.position.x);
				ImGui::Text("Camera Y: %f", camera.position.y);
				ImGui::Text("Camera Z: %f", camera.position.z);
				ImGui::Text("Camera Pitch: %f", glm::degrees(camera.pitch));
				ImGui::Text("Camera Yaw: %f", glm::degrees(camera.yaw));
				

				if (ImGui::Button("Recenter Camera")) {
					camera.position.x = 0.0f;
					camera.position.z = 0.0f;
					mouseDragX = 0.0f;
					mouseDragY = displayHeight;
				}
			}
			ImGui::End();
		}

		//render the new imgui frame
		renderer.RenderImGuiFrame();
		renderer.Update();
	}
	shaderHandler.Destroy();
	modelFactory.DeleteDataObjects();
	renderer.Destroy();

	return 0;
}

