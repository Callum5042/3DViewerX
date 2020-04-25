
#include "Application.h"
#include <SDL_syswm.h>
#include <fstream>
#include <SDL_messagebox.h>

#include "imgui.h"
#include "examples/imgui_impl_sdl.h"
#include "examples/imgui_impl_dx11.h"

bool Application::OnInitialise()
{
	m_Renderer = new Renderer(GetWindow());
	m_Renderer->Init();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplSDL2_InitForD3D(GetWindow()->GetWindow());
	ImGui_ImplDX11_Init(m_Renderer->GetDevice(), m_Renderer->GetDeviceContext());

	m_Model = new Model();
	m_Model->Load(m_Renderer->GetDevice(), m_Renderer->GetDeviceContext());

	return true;
}

void Application::OnUpdate()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplSDL2_NewFrame(GetWindow()->GetWindow());
	ImGui::NewFrame();

	m_Model->Update();
}

void Application::OnRender()
{
	m_Renderer->ClearScreen();

	// Shader thing
	m_Model->Render(m_Renderer->GetDeviceContext());

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Refresh screen
	m_Renderer->Render();
}

void Application::OnQuit()
{
	Exit();
}

void Application::OnResize(int width, int height)
{
	m_Renderer->Resize(width, height);
}