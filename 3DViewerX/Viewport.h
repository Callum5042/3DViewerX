#pragma once

#include "Renderer.h"

class Viewport
{
public:
	Viewport(Renderer* renderer);

	void Update();
	void Set();

	constexpr int GetWidth() { return m_Width; }
	constexpr int GetHeight() { return m_Height; }
	constexpr bool IsFocused() { return m_IsViewportFocused; }

private:
	Renderer* m_Renderer = nullptr;

	int m_Width = 800;
	int m_Height = 600;

	bool m_IsViewportFocused = false;
};