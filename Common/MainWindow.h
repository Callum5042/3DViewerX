#pragma once

#include <SDL_video.h>
#include <string>

class MainWindow
{
public:
	MainWindow();
	~MainWindow();

	bool Create(std::string&& title, int width, int height);
	void Destroy();

	inline SDL_Window* GetWindow() const { return m_WindowHandle; }

	int GetWidth();
	int GetHeight();

private:
	SDL_Window* m_WindowHandle = nullptr;
};