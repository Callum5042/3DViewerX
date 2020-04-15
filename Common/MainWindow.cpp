
#include "MainWindow.h"

namespace
{
	SDL_Window* SDLWindow(void* window)
	{
		return reinterpret_cast<SDL_Window*>(window);
	}
}

MainWindow::MainWindow() 
{
}

MainWindow::~MainWindow()
{
	Destroy();
}

bool MainWindow::Create(std::string&& title, int width, int height)
{
	m_WindowHandle = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (m_WindowHandle == nullptr)
	{
		return false;
	}

	return true;
}

void MainWindow::Destroy()
{
	SDL_DestroyWindow(m_WindowHandle);
}

int MainWindow::GetWidth()
{
	int width = 0;
	SDL_GetWindowSize(::SDLWindow(m_WindowHandle), &width, nullptr);
	return width;
}

int MainWindow::GetHeight()
{
	int height = 0;
	SDL_GetWindowSize(::SDLWindow(m_WindowHandle), nullptr, &height);
	return height;
}