#pragma once

#include <SDL.h>
#include <string>
#include <glm/glm.hpp>

class Display {
private:
	int m_width;
	int m_height;

	SDL_Window* m_window;
	SDL_GLContext m_context;

public:
	Display() = default;
	Display(std::string title, int width, int height);
	SDL_Window* GetWindow() const;
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	void Update();
	void Destroy();
};