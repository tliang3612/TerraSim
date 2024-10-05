#include <glad/glad.h>
#include <iostream>
#include "display.h"
#include <imgui/imgui.h>
#include <SDL.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glm/ext/matrix_clip_space.hpp>
#undef main // Fixes weird SDL issue


int main()
{
	const int displayWidth = 1280;
	const int displayHeight = 720;
	int width = 0;
	int height = 0;
	Display display = Display("TerraSim Prototype", displayWidth, displayHeight);
	SDL_GL_GetDrawableSize(display.GetWindow(), &width, &height);

	bool shouldRun = true;
	while (shouldRun) {

		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			ImGui_ImplSDL2_ProcessEvent(&e);

			if (e.type == SDL_QUIT) // handle quit event
				shouldRun = false;
		}
		// prepare the new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();


		//render the UI
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


		//clear color and depth buffer before rendering new frame
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//render the new frame
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		display.Update();
	}
	display.Destroy();

	return 0;
}

