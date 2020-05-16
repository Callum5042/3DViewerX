#include "FileLoader.h"
#include <Windows.h>
#include "imgui.h"

namespace
{
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
}

void FileLoader::FileTree(std::filesystem::path&& file_path)
{
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
					file_path = file;
				}

				ImGui::TreePop();
			}

			// get the next drive
			szSingleDrive += strlen(szSingleDrive) + 1;
		}
	}
}