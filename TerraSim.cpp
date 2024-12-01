#include <glad/glad.h>
#include <iostream>
#include <imgui/imgui.h>
#include <SDL.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <tinyfiledialogs/tinyfiledialogs.h>
#include "renderer.h"
#include "terrain.h"
#include "water.h"
#include "data_factory.h"
#include "shader_handlers/terrain_shader_handler.h"
#include "shader_handlers/shadowmap_shader_handler.h"
#include "shader_handlers/skybox_shader_handler.h"
#include "shader_handlers/water_shader_handler.h"
#include "camera.h"
#include "sculptor.hpp"
#include "raycaster.hpp"
#include "shadowmap.hpp"
#include "light.hpp"
#include <fstream>
#undef main // Fixes weird SDL issue

int main()
{
	const int displayWidth = 1280;
	const int displayHeight = 720;
	Renderer renderer = Renderer("TerraSim Prototype", displayWidth, displayHeight);

	//create the camera
	Camera camera = Camera(glm::vec3(0.0f, 300.f, 0.0f), 0.0f, 0.f, 0.0f);

	//init shaders
	TerrainShaderHandler terrainShaderHandler = TerrainShaderHandler();
	ShadowmapShaderHandler shadowmapShaderHandler = ShadowmapShaderHandler();
	SkyboxShaderHandler skyboxShaderHandler = SkyboxShaderHandler();
	WaterShaderHandler waterShaderHandler = WaterShaderHandler();
	DataFactory dataFactory = DataFactory();

	//user inputs
	bool keyW = false, keyA = false, keyS = false, keyD = false, keyQ = false, keyE = false;
	bool keyLeftShift = false, keyRightShift = false;
	float mouseDragX = 0.f, mouseDragY = displayHeight;
	int mouseX = 0.f, mouseY = 0.f;
	bool mouseLeft = false, mouseRight = false;
	float sculptRadius = 50.f;
	
	//light
	Light light(glm::vec3(0.f, 1.f, 0.f), glm::vec3(100000.f, 100000.f, 100000.f), .8f, 20.f, .5f);
	static float azimuthVal = 0.f; //horizontal rotation of light
	static float altitudeVal = 30.f; //vertical rotation of light
	static float brightness = 1.f;
	static float sunFalloff = 30.f;
	static float sunIntensity = .3f;

	//water
	static float moveFactor = 0.f;
	static float waterHeight = .5f;
	static float waterShininess;
	static bool waterEnabled = false;

	//terrain
	static float amplitude = 80.f;
	static float frequency = .2f;

	//load the textures
	static float texScaleVal = 50.f;
	GLuint textureID1 = dataFactory.LoadTexture("resources/Base.png");
	GLuint textureID2 = dataFactory.LoadTexture("resources/Ground.png");
	GLuint textureID3 = dataFactory.LoadTexture("resources/Rock.png");
	GLuint textureID4 = dataFactory.LoadTexture("resources/Peaks.png");
	std::vector<GLuint> textureIDs = { textureID1, textureID2, textureID3, textureID4 };

	GLuint skyboxTextureID = dataFactory.LoadCubemapTexture({
		"resources/skybox/right.png",
		"resources/skybox/left.png",
		"resources/skybox/top.png",
		"resources/skybox/bottom.png",
		"resources/skybox/back.png",
		"resources/skybox/front.png"
	});	

	GLuint dudvMapTextureID = dataFactory.LoadTexture("resources/water/waterNormal.png");
	GLuint normalmapTextureID = dataFactory.LoadTexture("resources/water/waterDUDV.png");
	
	float size = 256.f;
	int resolution = size * 2;

	//Terrain logic
	TerrainFactory terrainFactory = TerrainFactory();
	Terrain terrain = terrainFactory.GenerateTerrain(dataFactory, size, resolution, textureIDs);
	Heightmap& heightmap = terrain.GetHeightmap();


	//Skybox logic
	CubemapFactory cubemapFactory = CubemapFactory();
	Cubemap skybox = cubemapFactory.GenerateCubemap(dataFactory, skyboxTextureID);

	//Water logic
	WaterFactory waterFactory = WaterFactory();
	Water water = waterFactory.GenerateWater(dataFactory, dudvMapTextureID, normalmapTextureID, size, displayWidth, displayHeight);

	//for shadowmap
	static bool shadowmapDirty = true;

	auto deltaTimeCounter = SDL_GetPerformanceCounter(); //record the deltaTime counter
	auto fpsCounter = SDL_GetPerformanceCounter(); //record the fps counter
	auto ticksFrequency = SDL_GetPerformanceFrequency(); //performance counter ticks per second
	float fps = 0;
	float targetFrameTime = 1000.f / 325.f; //cap frame rate at 265. This is to ensure that extra resources aren't wasted on this application. Might Change
	bool shouldRun = true;

	while (shouldRun) {

		//FPS and DeltaTime Logic 
		auto currentCounter = SDL_GetPerformanceCounter(); //ticks as of now
		float deltaTime = static_cast<float>(currentCounter - deltaTimeCounter) / ticksFrequency; //difference between start and current ticks. Divide by frequency to get seconds
		deltaTimeCounter = currentCounter; //set start ticks to current ticks

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
				if (key == SDLK_q) keyQ = keyDown;
				if (key == SDLK_e) keyE = keyDown;
				if (key == SDLK_LSHIFT) keyLeftShift = keyDown;
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN) {
				auto button = e.button.button;
				if (button == SDL_BUTTON_LEFT) mouseLeft = true;
				else if (button == SDL_BUTTON_RIGHT) mouseRight = true;
			}
			else if (e.type == SDL_MOUSEBUTTONUP) {
				auto button = e.button.button;
				if (button == SDL_BUTTON_LEFT) mouseLeft = false;
				else if (button == SDL_BUTTON_RIGHT) mouseRight = false;
			}
			else if (e.type == SDL_MOUSEWHEEL) {
				float incrementVal = 200.f * deltaTime;
				if (e.wheel.y > 0) {
					sculptRadius += incrementVal;
				}
				else if (e.wheel.y < 0) {
					sculptRadius -= incrementVal;
				}
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

		camera.Update(deltaTime, keyW, keyA, keyS, keyD, keyQ, keyE, keyLeftShift, mouseDragX, mouseDragY, displayWidth, displayHeight);	

		//shadowmap
		Shadowmap shadowmap = terrain.GetShadowmap();

		//projectionMatrix
		glm::mat4 projectionMatrix = renderer.GetProjectionMatrix();
		glm::mat4 viewMatrix = camera.GetViewMatrix();

		//water
		float waterHeight = water.WaterHeight;
		moveFactor += water.WaveSpeed * deltaTime;
		if (moveFactor > 1.f) moveFactor = 0.f;
		glm::vec4 reflectionClip = glm::vec4(0.f, 1.f, 0.f, -waterHeight);
		glm::vec4 refractionClip = glm::vec4(0.f, -1.f, 0.f, waterHeight + 8.f);
		glm::vec4 defaultClip = glm::vec4(0.f);

		//render terrain to shadowmap if light has moved (shadowmapDirty)
		if(shadowmapDirty) {
			shadowmap.BindFrameBuffer();		
			glm::mat4 lightProjectionMatrix = light.GetProjectionMatrix();
			glm::mat4 lightViewMatrix = light.GetViewMatrix();
			shadowmapShaderHandler.Enable();
			shadowmapShaderHandler.SetLightViewProjection(lightProjectionMatrix * lightViewMatrix);
			renderer.RenderTerrain(terrain, terrainShaderHandler, shadowmap);
			shadowmapShaderHandler.Disable();
			shadowmap.UnbindFrameBuffer();
			shadowmapDirty = false;
		}

		// prepare the next main frame for rendering
		renderer.PrepareFrame();

		if (waterEnabled) {
			//reflection pass		
			water.BindFramebuffer(0);
			glm::mat4 waterViewMatrix = camera.GetReflectionViewMatrix(waterHeight);

			skyboxShaderHandler.Enable();
			skyboxShaderHandler.SetViewProjection(projectionMatrix * glm::mat4(glm::mat3(waterViewMatrix)));
			renderer.RenderSkybox(skybox, skyboxShaderHandler);
			skyboxShaderHandler.Disable();

			terrainShaderHandler.Enable();
			terrainShaderHandler.SetClip(reflectionClip);
			terrainShaderHandler.SetViewProjection((projectionMatrix * waterViewMatrix));
			renderer.RenderTerrain(terrain, terrainShaderHandler, shadowmap);
			terrainShaderHandler.Disable();
			water.UnbindFramebuffer();
			
			//refraction pass
			water.BindFramebuffer(1);

			skyboxShaderHandler.Enable();
			skyboxShaderHandler.SetViewProjection(projectionMatrix * glm::mat4(glm::mat3(viewMatrix)));
			renderer.RenderSkybox(skybox, skyboxShaderHandler);
			skyboxShaderHandler.Disable();

			terrainShaderHandler.Enable();
			terrainShaderHandler.SetClip(refractionClip);
			terrainShaderHandler.SetViewProjection(projectionMatrix * viewMatrix);
			renderer.RenderTerrain(terrain, terrainShaderHandler, shadowmap);
			terrainShaderHandler.Disable();

			water.UnbindFramebuffer();			
		}

		//lighting pass
		{
			skyboxShaderHandler.Enable();
			skyboxShaderHandler.SetLightDirection(light.lightDirection);
			skyboxShaderHandler.SetSunFalloff(light.sunFalloff);
			skyboxShaderHandler.SetSunIntensity(light.sunIntensity);
			skyboxShaderHandler.SetViewProjection(projectionMatrix* glm::mat4(glm::mat3(viewMatrix)));
			renderer.RenderSkybox(skybox, skyboxShaderHandler);
			skyboxShaderHandler.Disable();

			terrainShaderHandler.Enable();
			terrainShaderHandler.SetMinHeight(terrain.GetMinHeight());
			terrainShaderHandler.SetMaxHeight(terrain.GetMaxHeight());
			terrainShaderHandler.SetViewProjection(projectionMatrix* viewMatrix);
			terrainShaderHandler.SetLightDirection(light.lightDirection);
			terrainShaderHandler.SetBrightness(light.brightness);
			terrainShaderHandler.SetSunFalloff(light.sunFalloff);
			terrainShaderHandler.SetSunIntensity(light.sunIntensity);
			terrainShaderHandler.SetCameraPosition(camera.position);
			terrainShaderHandler.SetTextureScale(texScaleVal);
			terrainShaderHandler.SetClip(defaultClip);
			renderer.RenderTerrain(terrain, terrainShaderHandler, shadowmap);
			terrainShaderHandler.Disable();

			if (waterEnabled) {
				waterShaderHandler.Enable();
				waterShaderHandler.SetViewProjection(projectionMatrix* viewMatrix);
				waterShaderHandler.SetMoveFactor(moveFactor);
				waterShaderHandler.SetLightDirection(light.lightDirection);
				waterShaderHandler.SetCameraPosition(camera.position);
				waterShaderHandler.SetBrightness(light.brightness);
				waterShaderHandler.SetSunFalloff(light.sunFalloff);
				waterShaderHandler.SetSunIntensity(light.sunIntensity);
				waterShaderHandler.SetWaterHeight(waterHeight);
				waterShaderHandler.SetWaterShininess(waterShininess);
				renderer.RenderWater(water, waterShaderHandler);
				waterShaderHandler.Disable();
			}
		}

		//render the ImGUI
		renderer.PrepareImGuiFrame();
		{
			ImGui::Begin("Light Settings"); {
				ImGui::PushItemWidth(150);
				ImGui::SliderFloat("Light Azimuth", &azimuthVal, 0.0f, 360.f);
				ImGui::SliderFloat("Light Altitude", &altitudeVal, 0.0f, 360.f);
				ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.f);
				ImGui::SliderFloat("Sun Falloff", &sunFalloff, 0.01f, 100.f);
				ImGui::SliderFloat("Sun Intensity", &sunIntensity, 0.0f, 1.f);


				float horizontalScaling = std::cos(glm::radians(altitudeVal));
				float xAngle = std::cos(glm::radians(azimuthVal)) * horizontalScaling;
				float yAngle = std::sin(glm::radians(altitudeVal));
				float zAngle = std::sin(glm::radians(azimuthVal)) * horizontalScaling;

				glm::vec3 newLightDirection = glm::normalize(glm::vec3(xAngle, yAngle, zAngle));
				if (light.lightDirection != newLightDirection) {
					shadowmapDirty = true;
				}

				light.lightDirection = newLightDirection;
				light.brightness = brightness;
				light.sunFalloff = sunFalloff + 10.f;
				light.sunIntensity = sunIntensity;

				ImGui::PopItemWidth();
			}
			ImGui::End();

			ImGui::Begin("Terrain Generation Settings"); {
				ImGui::PushItemWidth(150);
				static const char* noiseTypes[] = {
					"Simplex Fractal",
					"fBm Mountain Range"
				};
				static int noiseType = 0;
				ImGui::Combo("Noise Type", &noiseType, noiseTypes, sizeof(noiseTypes) / sizeof(noiseTypes[0]));

				bool amplitudeChanged = ImGui::SliderFloat("Amplitude", &amplitude, 0.0f, 100.0f);
				bool frequencyChanged = ImGui::SliderFloat("frequency", &frequency, 0.0f, 1.0f);


				if (amplitudeChanged || frequencyChanged) {
					heightmap.Amplitude = amplitude;
					heightmap.Frequency = frequency;
					heightmap.GenerateHeightsUsingNoise(noiseType, false);
					heightmap.Update();
				}

				if (ImGui::Button("Generate Terrain")) {
					heightmap.Amplitude = amplitude;
					heightmap.Frequency = frequency;
					heightmap.GenerateHeightsUsingNoise(noiseType, true);
					heightmap.Update();
				}


				ImGui::PopItemWidth();
			}
			ImGui::End();

			ImGui::Begin("Brush Settings"); {
				ImGui::PushItemWidth(150);

				static const char* brushTypes[] = {
					"Step",
					"LinearFalloff",
					"Linear Smoothstep",
					"Polynomial",
					"Logarithmic"
				};
				static int brushType = 0;
				ImGui::Combo("Brush Type", &brushType, brushTypes, sizeof(brushTypes) / sizeof(brushTypes[0]));

				static float strength = .5f;
				ImGui::SliderFloat("Strength", &strength, 0.0f, 1.0f);

				static bool brushEnabled = true;
				ImGui::Checkbox("Enable Brush", &brushEnabled);

				if (brushEnabled) {
					float ndcX = (2.0f * mouseX) / displayWidth - 1.0f;
					float ndcY = 1.0f - (2.0f * mouseY) / displayHeight;

					glm::vec3 intersectionPoint = Raycaster::GetRaycastFromCameraToTerrain(camera.position, terrain, ndcX, ndcY, projectionMatrix, viewMatrix);

					if (intersectionPoint.y != std::numeric_limits<float>::max()) {
						if (!ImGui::GetIO().WantCaptureMouse) {
							terrainShaderHandler.Enable();
							terrainShaderHandler.SetIndicatorPosition(glm::vec2(intersectionPoint.x, intersectionPoint.z));
							terrainShaderHandler.SetIndicatorRadius(sculptRadius);
							terrainShaderHandler.Disable();

							if (mouseLeft) {
								Sculptor::Sculpt(heightmap, intersectionPoint.x, intersectionPoint.z, sculptRadius, strength * 90.f * deltaTime, brushType);
								terrain.Update();
							}
							else if (mouseRight) {
								Sculptor::Sculpt(heightmap, intersectionPoint.x, intersectionPoint.z, sculptRadius, -strength * 90.f * deltaTime, brushType);
								terrain.Update();
							}
						}
					}
				}
				else {
					terrainShaderHandler.Enable();
					terrainShaderHandler.SetIndicatorRadius(0);
					terrainShaderHandler.Disable();
				}

				ImGui::PopItemWidth();
			}
			ImGui::End();

			ImGui::Begin("Export"); {
				ImGui::PushItemWidth(150);
				if (ImGui::Button("Export to .obj")) {

				}



				ImGui::PopItemWidth();
			}
			ImGui::End();

			ImGui::Begin("Texture Settings"); {
				ImGui::PushItemWidth(150);

				std::vector<GLuint> textureIDs = terrain.GetTextureIDs();
				static GLuint textures[4] = { textureIDs[0],textureIDs[1],textureIDs[2], textureIDs[3]};
				const std::string textureNames[4] = { "Base Texture", "Ground Texture", "Rock Texture", "Peaks Texture" };

				// in scope function that will handle texture selection
				auto HandleTextureSelection = [&](int index) {
					// Open file dialog when button is clicked
					const char* filters[] = { "*.png", "*.jpg", "*.jpeg" };
					const char* filePath = tinyfd_openFileDialog("Select Texture", "", 3, filters, NULL, 0);

					if (filePath) {
						// Load the new texture
						GLuint newTextureID = dataFactory.LoadTexture(filePath); // Assumes you have a function to load textures
						textures[index] = newTextureID;

						// Update the texture in your terrain object or relevant structure
						terrain.UpdateTexture(index, newTextureID);
					}
				};

				for (int i = 0; i < 4; i++) {
					if (textures[i] != 0) {
						// imgui function that is able to take in a 
						if (ImGui::ImageButton((void*)(intptr_t)textures[i], ImVec2(50.f, 50.f))) {
							HandleTextureSelection(i);
						}
					}
					else {
						std::string buttonLabel = "Tex_" + std::to_string(i + 1);

						if (ImGui::Button(buttonLabel.c_str(), ImVec2(50.f, 50.f))) {
							HandleTextureSelection(i);
						}
					}

					ImGui::SameLine();
					ImGui::Text(textureNames[i].c_str());
				}

				ImGui::SliderFloat("Texture Scale", &texScaleVal, 10.0f, 300.f);

				ImGui::PopItemWidth();
			}
			ImGui::End();

			ImGui::Begin("Water Settings"); {
				ImGui::PushItemWidth(150);


				ImGui::SliderFloat("Water Level", &waterHeight, terrain.GetMinHeight(), terrain.GetMaxHeight());
				ImGui::SliderFloat("Specular Shininess", &waterShininess, 0.f, 100.f);
				ImGui::Checkbox("Enable Water", &waterEnabled);

				water.WaterHeight = waterHeight;
				water.WaterShininess = waterShininess;
			}
			ImGui::End();

			ImGui::Begin("Debug"); {
			    static int frameCount = 0; //increment frames
				float fpsElapsedTime = static_cast<float>(SDL_GetPerformanceCounter() - fpsCounter) / ticksFrequency; //delta time for fps calculations. Needs to be different var because fpsCounter 
				//will only update every 0.1s rather than each frame
				frameCount++; //increment frames
				if (fpsElapsedTime > 0.1f) { //update fps every .1s
					fps = (float)frameCount / fpsElapsedTime;
					fpsCounter = currentCounter;
					frameCount = 0;
				}

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

		//frames are moving too fast. pause until desired time 
		if (deltaTime < targetFrameTime) {
			SDL_Delay(static_cast<Uint32>(targetFrameTime - deltaTime));
		}
	}
	terrainShaderHandler.Destroy();
	dataFactory.DeleteDataObjects();
	renderer.Destroy();

	return 0;
}

