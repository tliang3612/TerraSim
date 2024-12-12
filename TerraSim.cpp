#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <filesystem>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>


#include <SDL.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tinyfiledialogs/tinyfiledialogs.h>

#include "engine/renderer.h"
#include "engine/data_factory.h"

#include "terrain/terrain.h"
#include "terrain/sculptor.hpp"

#include "effects/water.h"
#include "effects/shadowmap.hpp"
#include "effects/light.hpp"

#include "camera/camera.h"

#include "raycasting/raycaster.hpp"

#include "shader_handlers/terrain_shader_handler.h"
#include "shader_handlers/shadowmap_shader_handler.h"
#include "shader_handlers/skybox_shader_handler.h"
#include "shader_handlers/water_shader_handler.h"

#undef main // Fixes weird SDL issue

int main()
{
	const int displayWidth = 1280;
	const int displayHeight = 720;
	Renderer renderer = Renderer("TerraSim Prototype", displayWidth, displayHeight);

	//create the camera
	Camera camera = Camera(glm::vec3(0.f, 300.f, 0.f), 0.f, 0.f, 0.f);

	//init shaders
	TerrainShaderHandler terrainShaderHandler = TerrainShaderHandler();
	ShadowmapShaderHandler shadowmapShaderHandler = ShadowmapShaderHandler();
	SkyboxShaderHandler skyboxShaderHandler = SkyboxShaderHandler();
	WaterShaderHandler waterShaderHandler = WaterShaderHandler();
	DataFactory dataFactory = DataFactory();

	//user inputs
	bool keyW = false, keyA = false, keyS = false, keyD = false, keyQ = false, keyE = false;
	bool keyLeftShift = false, keyRightShift = false;
	float mouseDeltaX = 0.f, mouseDeltaY = displayHeight;
	int mouseX = 0.f, mouseY = 0.f;
	bool mouseLeft = false, mouseRight = false;
	float sculptRadius = 50.f;
	
	//light
	Light light(glm::vec3(0.f, 1.f, 0.f), glm::vec3(100000.f, 100000.f, 100000.f), .8f, 20.f, .5f, glm::vec3(1.f, 0.9f, 0.7f));
	static float azimuthVal = 0.f; //horizontal rotation of light
	static float altitudeVal = 30.f; //vertical rotation of light
	static float brightness = 1.f;
	static float sunFalloff = 30.f;
	static float sunIntensity = .3f;


	//load the textures
	std::string workingDirectory = std::filesystem::current_path().string();
	std::replace(workingDirectory.begin(), workingDirectory.end(), '\\', '/');
	static float texScaleVal = 25.f;
	GLuint textureID1 = dataFactory.LoadTexture(workingDirectory + "/resources/Base.png");
	GLuint textureID2 = dataFactory.LoadTexture(workingDirectory + "/resources/Ground.png");
	GLuint textureID3 = dataFactory.LoadTexture(workingDirectory + "/resources/Rock.png");
	GLuint textureID4 = dataFactory.LoadTexture(workingDirectory + "/resources/Peaks.png");
	std::vector<GLuint> textureIDs = { textureID1, textureID2, textureID3, textureID4 };

	GLuint skyboxTextureID = dataFactory.LoadCubemapTexture({
		workingDirectory + "/resources/skybox/right.png",
		workingDirectory + "/resources/skybox/left.png",
		workingDirectory + "/resources/skybox/top.png",
		workingDirectory + "/resources/skybox/bottom.png",
		workingDirectory + "/resources/skybox/back.png",
		workingDirectory + "/resources/skybox/front.png"
	});	

	GLuint dudvMapTextureID = dataFactory.LoadTexture(workingDirectory + "/resources/water/waterNormal.png");
	GLuint normalmapTextureID = dataFactory.LoadTexture(workingDirectory + "/resources/water/waterDUDV.png");
	
	//terrain size
	static int terrainSize = 256;
	static int oldTerrainSize = terrainSize;
	int terrainResolution = terrainSize * 2;

	//Terrain logic
	float noiseSeed = time(nullptr);
	TerrainFactory terrainFactory = TerrainFactory();
	Terrain terrain = terrainFactory.GenerateTerrain(dataFactory, terrainSize, terrainResolution, textureIDs, noiseSeed);
	std::shared_ptr<Heightmap> heightmap = terrain.GetHeightmap();


	//Skybox logic
	CubemapFactory cubemapFactory = CubemapFactory();
	Cubemap skybox = cubemapFactory.GenerateCubemap(dataFactory, skyboxTextureID);

	//Water logic
	WaterFactory waterFactory = WaterFactory();
	Water water = waterFactory.GenerateWater(dataFactory, dudvMapTextureID, normalmapTextureID, terrainSize, displayWidth, displayHeight);

	//for shadowmap
	static bool shadowmapDirty = true;

	//for water
	static float moveFactor = 0.f;
	static bool waterEnabled = true;

	//for terrain
	static float amplitude = heightmap->Amplitude;
	static float frequency = heightmap->Frequency;

	auto deltaTimeCounter = SDL_GetPerformanceCounter(); //record the deltaTime counter
	auto fpsCounter = SDL_GetPerformanceCounter(); //record the fps counter
	auto ticksFrequency = SDL_GetPerformanceFrequency(); //performance counter ticks per second
	float fps = 0;
	const float maxDeltaTime = 1000.f / 250.f; //cap frame rate at 250. This is to ensure that extra resources aren't wasted on this application.
	bool shouldRun = true;

	while (shouldRun) {

		//FPS and DeltaTime Logic 
		auto currentCounter = SDL_GetPerformanceCounter(); //ticks as of now
		float deltaTime = static_cast<float>(currentCounter - deltaTimeCounter) / ticksFrequency; //difference between start and current ticks. Divide by frequency to get seconds
		deltaTimeCounter = currentCounter; //set start ticks to current ticks

		static int frameCount = 0; //increment frames
		float fpsElapsedTime = static_cast<float>(currentCounter - fpsCounter) / ticksFrequency; //delta time for fps calculations. Needs to be different var because fpsCounter 
		//will only update every 0.2s rather than each frame
		frameCount++; //increment frames
		if (fpsElapsedTime > 0.2f) { //update fps every .2s
			fps = (float)frameCount / fpsElapsedTime;
			fpsCounter = currentCounter;
			frameCount = 0;
		}

		//frames are moving too fast. pause until desired time 
		if (deltaTime < maxDeltaTime) {
			SDL_Delay(static_cast<Uint32>(maxDeltaTime - deltaTime));
		}

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
					mouseDeltaX += e.motion.xrel;
					mouseDeltaY += e.motion.yrel;
				}
			} 
		}

		camera.Update(deltaTime, keyW, keyA, keyS, keyD, keyQ, keyE, keyLeftShift, mouseDeltaX, mouseDeltaY, displayWidth, displayHeight);	

		//shadowmap
		Shadowmap shadowmap = terrain.GetShadowmap();

		//projectionMatrix
		glm::mat4 projectionMatrix = renderer.GetProjectionMatrix();
		glm::mat4 viewMatrix = camera.GetViewMatrix();

		//water
		moveFactor += water.WaveSpeed * deltaTime;
		if (moveFactor > 1.f) moveFactor = 0.f;
		glm::vec4 reflectionClip = glm::vec4(0.f, 1.f, 0.f, -water.WaterHeight);
		glm::vec4 refractionClip = glm::vec4(0.f, -1.f, 0.f, water.WaterHeight + 12.f);
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
			glm::mat4 waterViewMatrix = camera.GetReflectionViewMatrix(water.WaterHeight);

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
			skyboxShaderHandler.SetSunColor(light.sunColor);
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
			terrainShaderHandler.SetSunColor(light.sunColor);
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
				waterShaderHandler.SetSunColor(light.sunColor);
				waterShaderHandler.SetSunIntensity(light.sunIntensity);
				waterShaderHandler.SetWaterHeight(water.WaterHeight);
				waterShaderHandler.SetWaterShininess(water.WaterShininess);
				renderer.RenderWater(water, waterShaderHandler);
				waterShaderHandler.Disable();
			}
		}

		//render the ImGUI
		renderer.PrepareImGuiFrame();
		{
			ImGui::Begin("Light Settings"); {
				ImGui::PushItemWidth(150);
				ImGui::SliderFloat("Light Azimuth", &azimuthVal, 0.f, 360.f);
				ImGui::SliderFloat("Light Altitude", &altitudeVal, 0.f, 360.f);
				ImGui::SliderFloat("Brightness", &brightness, 0.f, 1.f);
				ImGui::SliderFloat("Sun Falloff", &sunFalloff, 0.01f, 100.f);
				ImGui::SliderFloat("Sun Intensity", &sunIntensity, 0.f, 1.f);

				ImGui::ColorEdit3("Sun Color", glm::value_ptr(light.sunColor));

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

				bool amplitudeChanged = ImGui::SliderFloat("Amplitude", &amplitude, 0.f, 100.f);
				bool frequencyChanged = ImGui::SliderFloat("Frequency", &frequency, 0.f, 1.f);

				ImGui::InputInt("Terrain Size", &terrainSize);
				terrainSize = std::clamp(terrainSize, 0, 1000);

				static bool keepSeed = true;
				ImGui::Checkbox("Keep Noise Seed", &keepSeed);

				float noiseSeed = rand();
				if (keepSeed) {
					noiseSeed = heightmap->GetNoiseSeed();
				}

				if (amplitudeChanged || frequencyChanged) {
					heightmap->Amplitude = amplitude;
					heightmap->Frequency = frequency;
					heightmap->GenerateHeightsUsingNoise(noiseType, heightmap->GetNoiseSeed());
					heightmap->Update();
				}

				if (ImGui::Button("Generate Terrain")) {
					if (oldTerrainSize != terrainSize) {
						oldTerrainSize = terrainSize;
						terrain = terrainFactory.GenerateTerrain(dataFactory, terrainSize, terrainSize * 2, terrain.GetTextureIDs(), noiseSeed);
						water = waterFactory.GenerateWater(dataFactory, dudvMapTextureID, normalmapTextureID, terrainSize, displayWidth, displayHeight);
						heightmap = terrain.GetHeightmap();
					}
					else {
						heightmap->Amplitude = amplitude;
						heightmap->Frequency = frequency;
						heightmap->GenerateHeightsUsingNoise(noiseType, noiseSeed);
						heightmap->Update();
					}
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
				ImGui::SliderFloat("Strength", &strength, 0.f, 1.f);

				static bool brushEnabled = true;
				ImGui::Checkbox("Enable Brush", &brushEnabled);

				if (brushEnabled) {
					float ndcX = (2.f * mouseX) / displayWidth - 1.f;
					float ndcY = 1.f - (2.f * mouseY) / displayHeight;

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

			ImGui::Begin("Export");
			{
				ImGui::PushItemWidth(150);

				static bool isExporting = false;
				static float progress = 0.f;

				if (ImGui::Button("Export to .obj")) {
					const char* filterPatterns[] = { "*.obj" };

					//open save file dialog using tiny file dialog
					const char* filePath = tinyfd_saveFileDialog(
						"Export Wavefront File",
						"terrain.obj",
						1,
						filterPatterns,
						"Wavefront File"
					);

					if (filePath) {
						isExporting = true;
						progress = 0.f;

						ImGui::OpenPopup("Exporting");

						//create a thread for the exporting the heightmap
						std::thread([&]() {
							std::ostringstream output;

							std::vector<int> indices = terrain.GetIndices();

							std::ofstream file(filePath);
							if (!file.is_open()) {
								std::cerr << "Error trying to open file for writing: " << filePath << std::endl;
								isExporting = false;
								return 0;
							}

							float step = (2.f * terrainSize) / (terrainResolution - 1);

							//for progress bar
							int totalSteps = terrainResolution * terrainResolution + indices.size() / 3;
							int currentStep = 0;

							for (int z = 0; z < terrainResolution; z++) {
								for (int x = 0; x < terrainResolution; ++x) {
									float posX = -terrainSize + x * step;
									float posZ = -terrainSize + z * step;
									float posY = heightmap->GetHeight(x, z); //height from heightmap
									output << "v " << posX << " " << posY << " " << posZ << "\n";

									float u = float(x) / (terrainResolution - 1);
									float v = float(z) / (terrainResolution - 1);
									output << "vt " << u << " " << v << "\n";

									//progress update
									currentStep++;
									progress = float(currentStep) / totalSteps;
								}
							}

							for (int i = 0; i < indices.size(); i += 3) {
								int corner1 = indices[i] + 1;
								int corner2 = indices[i + 1] + 1;
								int corner3 = indices[i + 2] + 1;

								output << "f " << corner1 << "/" << corner1 << " "
									           << corner2 << "/" << corner2 << " "
									           << corner3 << "/" << corner3 << "\n";

								//progress update
								currentStep++;
								progress = float(currentStep) / totalSteps;
							}

							file << output.str();
							file.close();

							isExporting = false;
							}).detach();
					}
				}

				//catch open popup signal from imgui. if caught, start progress bar
				if (ImGui::BeginPopupModal("Exporting", nullptr)) {
					if (isExporting) {
						ImGui::Text("Exporting terrain to .obj...");
						ImGui::ProgressBar(progress, ImVec2(ImGui::GetWindowWidth() - 20.f, 0.f));
					}
					else {
						ImGui::Text("Export completed!");
						if (ImGui::Button("Close")) {
							ImGui::CloseCurrentPopup();
						}
					}
					ImGui::EndPopup();
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

				ImGui::SliderFloat("Texture Scale", &texScaleVal, 10.f, 300.f);

				ImGui::PopItemWidth();
			}
			ImGui::End();

			ImGui::Begin("Water Settings"); {
				ImGui::PushItemWidth(150);


				ImGui::SliderFloat("Water Level", &water.WaterHeight, terrain.GetMinHeight(), terrain.GetMaxHeight());
				ImGui::SliderFloat("Specularity", &water.WaterShininess, 0.f, 100.f);
				ImGui::Checkbox("Enable Water", &waterEnabled);
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
					camera.position.x = 0.f;
					camera.position.z = 0.f;
					mouseDeltaX = 0.f;
					mouseDeltaY = displayHeight;
				}
			}
			ImGui::End();
		}

		//render the new imgui frame
		renderer.RenderImGuiFrame();
		renderer.Update();


	}
	terrainShaderHandler.Destroy();
	dataFactory.DeleteDataObjects();
	renderer.Destroy();

	return 0;
}

