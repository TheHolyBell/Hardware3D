#include "App.h"
#include "Timer.h"
#include "Pyramid.h"
#include "Box.h"
#include <algorithm>
#include "Surface.h"
#include "GDIPlusManager.h"
#include "SkinnedBox.h"
#include "ImGui\imgui.h"
#include "ImGui\imgui_impl_win32.h"
#include "ImGui\imgui_impl_dx11.h"
#include "Cylinder.h"

GDIPlusManager g_GDIPlusManager;

#define PI 3.14159

class Factory
{
	Graphics& gfx;
	std::mt19937 rng{ std::random_device{}() };
	std::uniform_int_distribution<int> sdist{ 0,3 };
	std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
	std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
	std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
	std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
	std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
	std::uniform_real_distribution<float> cdist{ 0.0f,1.0f };
	std::uniform_int_distribution<int> tdist{ 3,30 };
public:
	Factory(Graphics& gfx)
		: gfx(gfx){}
	std::unique_ptr<Drawable> operator()()
	{
		DirectX::XMFLOAT3 material = { cdist(rng), cdist(rng), cdist(rng) };

		switch (sdist(rng))
		{
		case 0:
			return std::make_unique<Box>(gfx,
				rng, adist, ddist, odist,
				rdist, bdist, material);
		case 1:
			return std::make_unique<Cylinder>(gfx, rng,
				adist, ddist, odist, rdist, bdist, tdist
				);
		case 2:
			return std::make_unique<Pyramid>(gfx, rng,
				adist, ddist, odist, rdist, tdist);
		case 3:
			return std::make_unique<SkinnedBox>(gfx, rng,
				adist, ddist, odist, rdist);
		default:
			assert(false && "Incorrect value in switch");
			return nullptr;
		}
	}
};

App::App(int Width, int Height, const std::string& title)
	: m_Window(Width, Height, title.c_str()), m_Light(m_Window.Gfx())
{
	Factory f(m_Window.Gfx());
	m_Drawables.reserve(NumberDrawables);

	std::generate_n(std::back_inserter(m_Drawables), NumberDrawables, f);

	// Init box pointers for editing instance parameters
	for (const auto& d : m_Drawables)
	{
		if (auto pt = dynamic_cast<Box*>(d.get()))
			m_Boxes.push_back(pt);
	}
}

int App::Go()
{
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		DoFrame();
	}

	return msg.wParam;
}

void App::DoFrame()
{
	static float _speedFactor = 1.0f;
	Graphics& gfx = m_Window.Gfx();

	auto dt = m_Timer.Mark() * _speedFactor;


	gfx.BeginFrame(0.07f, 0.0f, 0.12f);
	gfx.SetCamera(m_Camera.GetMatrix());
	m_Light.Bind(gfx, gfx.GetCamera());

	for (const auto& d : m_Drawables)
	{
		d->Update(m_Window.g_Keyboard.KeyIsPressed(VK_SPACE) ? 0 : dt);
		d->Draw(gfx);
	}

	static char _buffer[1024];

	// ImGui window to control simulation speed
	if (ImGui::Begin("Simulation Speed"))
	{
		ImGui::SliderFloat("Speed Factor", &_speedFactor, 0.0f, 4.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::InputText("Butts", _buffer, sizeof(_buffer));
	}
	ImGui::End();

	m_Camera.SpawnControlWindow();
	m_Light.SpawnControlWindow();
	SpawnBoxWindowManagerWindow();
	SpawnBoxWindows();

	gfx.EndFrame();
}

void App::SpawnBoxWindowManagerWindow() noexcept
{
	if (ImGui::Begin("Boxes"))
	{
		using namespace std::string_literals;
		const auto preview = m_ComboBoxIndex ? std::to_string(*m_ComboBoxIndex) : "Choose a box..."s;
		if (ImGui::BeginCombo("Box Number", preview.c_str()))
		{
			for (int i = 0; i < m_Boxes.size(); i++)
			{
				const bool selected = *m_ComboBoxIndex == i;
				if (ImGui::Selectable(std::to_string(i).c_str(), selected))
				{
					m_ComboBoxIndex = i;
				}
				if (selected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button("Spawn Control Window") && m_ComboBoxIndex)
		{
			m_BoxControlIds.insert(*m_ComboBoxIndex);
			m_ComboBoxIndex.reset();
		}
	}
	ImGui::End();
}

void App::SpawnBoxWindows()
{
	for (auto i = m_BoxControlIds.begin(); i != m_BoxControlIds.end(); )
	{
		if (!m_Boxes[*i]->SpawnControlWindow(*i, m_Window.Gfx()))
		{
			i = m_BoxControlIds.erase(i);
		}
		else
		{
			i++;
		}
	}
}
