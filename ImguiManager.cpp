#include "ImguiManager.h"
#include "ImGui\imgui.h"
#include <filesystem>

ImguiManager::ImguiManager()
{
	if (!std::filesystem::exists("imgui.ini") && std::filesystem::exists("imgui_default.ini"))
		std::filesystem::copy_file("imgui_default.ini", "imgui.ini");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	auto& context = ImGui::GetIO();
	context.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

ImguiManager::~ImguiManager()
{
	ImGui::DestroyContext();
}
