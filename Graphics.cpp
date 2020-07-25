#include "Graphics.h"

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")

Graphics::Graphics(HWND hwnd, int Width, int Height)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = Width;
	sd.BufferDesc.Height = Height;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.SampleDesc.Count = 4;
	sd.SampleDesc.Quality = 0;

	D3D11CreateDeviceAndSwapChain(nullptr,
		D3D_DRIVER_TYPE_HARDWARE, nullptr,
		0, nullptr, 0, D3D11_SDK_VERSION, &sd,
		&m_pSwapChain, &m_pDevice, nullptr, &m_pImmediateContext);

	Microsoft::WRL::ComPtr<ID3D11Texture2D> _pBackBuffer = nullptr;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &_pBackBuffer);

	m_pDevice->CreateRenderTargetView(_pBackBuffer.Get(), nullptr, &m_pRTV);
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	float _color[] = { red, green, blue, 1.0f };
	m_pImmediateContext->ClearRenderTargetView(m_pRTV.Get(), _color);
}

void Graphics::EndFrame()
{
	m_pSwapChain->Present(1, 0);
}
