#include "ImguiManager.h"
#include "ImGui\imgui.h"

ImguiManager::ImguiManager()
{
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
