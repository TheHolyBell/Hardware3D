#pragma once
#include "Window.h"
#include <string>
#include "Drawable.h"
#include "Timer.h"
#include "ImguiManager.h"
#include "Camera.h"
#include "PointLight.h"
#include <set>
#include <optional>

class App
{
public:
	App(int Width = 800, int Height = 600, const std::string& title = "Direct3D");
	int Go();
private:
	void DoFrame();
	void SpawnBoxWindowManagerWindow() noexcept;
	void SpawnBoxWindows();
private:
	ImguiManager m_ImGuiManager;
	Window m_Window;
	Camera m_Camera;
	Timer m_Timer;
	std::vector<std::unique_ptr<Drawable>> m_Drawables;
	std::vector<class Box*> m_Boxes;
	PointLight m_Light;
	std::optional<int> m_ComboBoxIndex;
	std::set<int> m_BoxControlIds;
	static constexpr size_t NumberDrawables = 180;
};