
#include "Application.h"
#include <SDL_syswm.h>
#include <SDL_messagebox.h>

#include "imgui.h"
#include "examples/imgui_impl_sdl.h"
#include "examples/imgui_impl_dx11.h"

#include <DDSTextureLoader.h>
#include "FileLoader.h"

#include <iostream>

bool Application::OnInitialise()
{
	m_Renderer = new Renderer(GetWindow());
	m_Renderer->Init();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplSDL2_InitForD3D(GetWindow()->GetWindow());
	ImGui_ImplDX11_Init(m_Renderer->GetDevice(), m_Renderer->GetDeviceContext());

	ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

	m_Viewport = new Viewport(m_Renderer);
	m_Camera = new Camera();
	m_Model = new Model(m_Renderer);
	m_Model->Load("D:\\3d models\\crate\\crate.obj");

	m_Timer = new Timer();
	m_Timer->Reset();

	return true;
}

void Application::OnUpdate()
{
	m_Timer->Tick();
	CalculateFrameStats();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplSDL2_NewFrame(GetWindow()->GetWindow());
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();

	// Example layout
	float width = static_cast<float>(GetWindow()->GetWidth());
	float height = static_cast<float>(GetWindow()->GetHeight());

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::GetStyle().WindowRounding = 0.0f;

	bool open = true;
	if (ImGui::Begin("Example: Simple layout", &open, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar))
	{
		// Menu bar
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::MenuItem("Close");
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::BeginChild("Load", ImVec2(300, 0), true);

		ImGui::BeginGroup();
		ImGui::BeginChild("item view"); // Leave room for 1 line below us
		if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
		{
			// File loader
			if (ImGui::BeginTabItem("Load"))
			{
				std::filesystem::path path;
				FileLoader::FileTree(std::move(path));

				if (!path.empty())
				{
					std::cout << "Load file\n";
					if (m_Model->IsLoaded())
					{
						m_Model->Unload();
					}

					m_Model->Load(path.string());
				}

				ImGui::EndTabItem();
			}

			// Model Details
			if (ImGui::BeginTabItem("Details"))
			{
				ImGui::Text("Model");
				if (m_Model->IsLoaded())
				{
					m_Model->Gui();
				}

				ImGui::Spacing();
				ImGui::Separator();

				ImGui::Text("Camera");
				m_Camera->Update();

				ImGui::Spacing();
				ImGui::Separator();

				ImGui::Text("Lighting");
				ImGui::SliderFloat("LX-Axis", &m_Renderer->m_LightDirX, -1.0f, 1.0f);
				ImGui::SliderFloat("LY-Axis", &m_Renderer->m_LightDirY, -1.0f, 1.0f);
				ImGui::SliderFloat("LZ-Axis", &m_Renderer->m_LightDirZ, -1.0f, 1.0f);

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
		ImGui::EndChild();
		ImGui::EndGroup();
		ImGui::EndChild();
		ImGui::SameLine();

		// Right
		{
			m_Viewport->Update();
		}
	}

	ImGui::End();

	if (m_Model->IsLoaded())
	{
		m_Model->Update();
	}
}

void Application::OnRender()
{
	m_Renderer->ClearScreen();

	// Draw Viewport

	m_Viewport->Set();
	if (m_Model->IsLoaded())
	{
		m_Model->Render();
	}

	// Draw GUI
	m_Renderer->SetWindowTarget();
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

void Application::CalculateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((m_Timer->TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		m_FPS = fps;

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}