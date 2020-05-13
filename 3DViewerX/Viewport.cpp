#include "Viewport.h"
#include <imgui.h>
#include <iostream>
Viewport::Viewport(Renderer* renderer) : m_Renderer(renderer)
{
}

void Viewport::Update()
{
	ImGui::Begin("Viewport");
	m_IsViewportFocused = ImGui::IsWindowFocused();

	// Draw scene to viewport
	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImVec2 size = ImGui::GetContentRegionAvail();
	int width = (int)size.x;
	int height = (int)size.y;

	if (m_Width != width || m_Height != height)
	{
		m_Width = width;
		m_Height = height;

		m_Renderer->ResizeViewport(width, height);

		m_IsViewportFocused = false;
	}

	ID3D11ShaderResourceView* view = m_Renderer->GetTextureMap();
	ImGui::GetWindowDrawList()->AddImage((void*)view, pos, ImVec2(pos.x + width, pos.y + height));

	ImGui::End();
}

void Viewport::Set()
{
	m_Renderer->SetViewportTarget(m_Width, m_Height);
}