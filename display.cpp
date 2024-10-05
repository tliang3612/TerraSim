#include <glad/glad.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl2.h>
#include "display.h"
#include "util/util.h"

Display::Display(std::string display_name, int width, int height)
{
	m_width = width;
	m_height = height;

	// Initi SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		util::fatal_error(("SDL_Init: %s", SDL_GetError()));
	}
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

	// Create the SDL window*.
	m_window = SDL_CreateWindow(
		display_name.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		m_width,
		m_height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
	);
	if (!m_window) {
		util::fatal_error("SDL_CreateWindow: %s", SDL_GetError());
	}

	m_context = SDL_GL_CreateContext(m_window);
	if (!m_context) {
		util::fatal_error("SDL_GL_CreateContext: %s", SDL_GetError());
	}

	gladLoadGLLoader(SDL_GL_GetProcAddress);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsClassic();

	ImGui_ImplSDL2_InitForOpenGL(m_window, m_context);
	ImGui_ImplOpenGL3_Init("#version 330");
}

SDL_Window* Display::GetWindow() const
{
	return m_window;
}

// update the display.
void Display::Update() {
	SDL_GL_SwapWindow(m_window);
}

// destroy the display.
void Display::Destroy() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}
