#include "App.h"
#include "Timer.h"
#include "Melon.h"
#include "Pyramid.h"
#include "Box.h"
#include <algorithm>

#define PI 3.14159

class Factory
{
	Graphics& gfx;
	std::mt19937 rng{ std::random_device{}() };
	std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
	std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
	std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
	std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
	std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
	std::uniform_int_distribution<int> latdist{ 5,20 };
	std::uniform_int_distribution<int> longdist{ 10,40 };
	std::uniform_int_distribution<int> typedist{ 0,2 };
public:
	Factory(Graphics& gfx)
		: gfx(gfx){}
	std::unique_ptr<Drawable> operator()()
	{
		switch (typedist(rng))
		{
		case 0:
			return std::make_unique<Pyramid>(
				gfx, rng, adist, ddist, odist, rdist);
		case 1:
			return std::make_unique<Box>(
				gfx, rng, adist, ddist, odist, rdist, bdist);
		case 2:
			return std::make_unique<Melon>(
				gfx, rng, adist, ddist,
				odist, rdist, longdist, latdist);
		default:
			assert(false && "Incorrect value in factory");
			return nullptr;
		}
	}
};

App::App(int Width, int Height, const std::string& title)
	: m_Window(Width, Height, title.c_str())
{
	Factory f(m_Window.Gfx());
	m_Drawables.reserve(NumberDrawables);

	std::generate_n(std::back_inserter(m_Drawables), NumberDrawables, f);

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
	for (auto& d : m_Drawables)
	{
		d->Update(dt);
		d->Draw(m_Window.Gfx());
	}
	m_Window.Gfx().EndFrame();
}
