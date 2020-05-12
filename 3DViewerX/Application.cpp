
#include "Application.h"
#include <SDL_syswm.h>
#include <fstream>
#include <SDL_messagebox.h>

#include "imgui.h"
#include "examples/imgui_impl_sdl.h"
#include "examples/imgui_impl_dx11.h"

#include <filesystem>
#include <iostream>

#include <DDSTextureLoader.h>
#include <Windows.h>

bool Application::OnInitialise()
{
	m_Renderer = new Renderer(GetWindow());
	m_Renderer->Init();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplSDL2_InitForD3D(GetWindow()->GetWindow());
	ImGui_ImplDX11_Init(m_Renderer->GetDevice(), m_Renderer->GetDeviceContext());

	m_Viewport = new Viewport(m_Renderer);
	m_Camera = new Camera();
	m_Model = new Model(m_Renderer);
	m_Model->Load("C:\\Users\\Callum\\Desktop\\3d models\\cube\\cube.obj");

	return true; 
}

void FileDirectory(std::filesystem::path entry_path, std::filesystem::path* file)
{
	std::filesystem::directory_options options = std::filesystem::directory_options::skip_permission_denied;
	for (const auto& entry : std::filesystem::directory_iterator(entry_path, options))
	{
		const auto path = entry.path();

		if (entry.is_directory())
		{
			if (ImGui::TreeNode(path.string().c_str()))
			{
				FileDirectory(path, file);
				ImGui::TreePop();
			}
		}
		else if (entry.is_regular_file())
		{
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
			ImGui::TreeNodeEx(path.filename().string().c_str(), node_flags);
			if (ImGui::IsItemClicked())
			{
				const std::string extensions = path.extension().string();
				if (extensions == ".obj")
				{
					*file = path;
				}
			}
		}
	}
}

void Application::OnUpdate()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplSDL2_NewFrame(GetWindow()->GetWindow());
	ImGui::NewFrame();

	//bool demo;
	//ImGui::ShowDemoWindow(&demo);

	// Model loader
	ImGui::Begin("Load");

	// Get drives
	DWORD dwSize = MAX_PATH;
	char szLogicalDrives[MAX_PATH] = { 0 };
	DWORD dwResult = GetLogicalDriveStringsA(dwSize, szLogicalDrives);

	// Show directory tree
	if (dwResult > 0 && dwResult <= MAX_PATH)
	{
		char* szSingleDrive = szLogicalDrives;
		while (*szSingleDrive)
		{
			if (ImGui::TreeNode(szSingleDrive))
			{
				std::filesystem::path path(szSingleDrive);

				std::filesystem::path file;
				FileDirectory(path, &file);
				if (std::filesystem::is_regular_file(file))
				{
					std::cout << "Load file\n";
					if (m_Model->IsLoaded())
					{
						// Unload
						m_Model->Unload();
					}

					m_Model->Load(file.string());
				}

				ImGui::TreePop();
			}

			// get the next drive
			szSingleDrive += strlen(szSingleDrive) + 1;
		}
	}

	ImGui::End();

	m_Camera->Update();
	m_Viewport->Update();

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