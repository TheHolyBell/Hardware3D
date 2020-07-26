#include "App.h"
#include "Timer.h"
#include <iostream>

App::App(int Width, int Height, const std::string& title)
	: m_Window(Width, Height, title.c_str())
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> adist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> ddist(0.0f, 3.1415f * 2.0f);
	std::uniform_real_distribution<float> odist(0.0f, 3.1415f * 0.3f);
	std::uniform_real_distribution<float> rdist(6.0f, 20.0f);
	for (auto i = 0; i < 80; i++)
	{
		m_Boxes.push_back(std::make_unique<Box>(
			m_Window.Gfx(), rng, adist,
			ddist, odist, rdist
			));
	}
	m_Window.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
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
	auto dt = m_Timer.Mark();
	m_Window.Gfx().ClearBuffer(0.07f, 0.0f, 0.12f);
	for (auto& b : m_Boxes)
	{
		b->Update(dt);
		b->Draw(m_Window.Gfx());
	}
	m_Window.Gfx().EndFrame();
}
