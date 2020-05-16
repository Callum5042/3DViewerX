#include "Viewport.h"
#include <imgui.h>
#include <iostream>
#include "Camera.h"
#include "Application.h"

Viewport::Viewport(Renderer* renderer) : m_Renderer(renderer)
{
}

void Viewport::Update()
{
	ImGui::BeginChild("Viewport");
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

		Camera* camera = reinterpret_cast<Application*>(Application::GetInstance())->GetCamera();
		camera->Resize();

		m_IsViewportFocused = false;
	}

	if (m_WindowPosX != pos.x || m_WindowPosY != pos.y)
	{
		m_WindowPosX = pos.x;
		m_WindowPosY = pos.y;

		m_IsViewportFocused = false;
	}

	ID3D11ShaderResourceView* view = m_Renderer->GetTextureMap();
	ImGui::GetWindowDrawList()->AddImage((void*)view, pos, ImVec2(pos.x + width, pos.y + height));

	// FPS overlay
	const float DISTANCE = 10.0f;
	bool open = true;
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	ImGui::SetNextWindowPos(ImVec2(m_WindowPosX + DISTANCE, m_WindowPosY + DISTANCE), ImGuiCond_Always);
	if (ImGui::Begin("Example: Simple overlay", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
	{
		int fps = static_cast<int>(reinterpret_cast<Application*>(Application::GetInstance())->GetFPS());

		std::string fps_str("FPS: " + std::to_string(fps));
		ImGui::Text(fps_str.c_str());
	}

	ImGui::End();

	ImGui::EndChild();
}

void Viewport::Set()
{
	m_Renderer->SetViewportTarget(m_Width, m_Height);
}