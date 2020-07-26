#include "Graphics.h"
#include "dxerr.h"
#include <sstream>
#include "Window.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "GraphicsThrowMacros.h"

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace dx = DirectX;


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
	//sd.OutputWindow = hwnd;
	sd.OutputWindow = hwnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.SampleDesc.Count = 4;
	sd.SampleDesc.Quality = 0;

	UINT swapCreateFlags = 0u;
#ifndef NDEBUG
	swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// for checking results of d3d functions
	HRESULT hr;

	// create device and front/back buffers, and swap chain and rendering context
	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&m_pSwapChain,
		&m_pDevice,
		nullptr,
		&m_pImmediateContext
	));

	Microsoft::WRL::ComPtr<ID3D11Texture2D> _pBackBuffer = nullptr;
	GFX_THROW_INFO(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &_pBackBuffer));
	GFX_THROW_INFO(m_pDevice->CreateRenderTargetView(_pBackBuffer.Get(), nullptr, &m_pRTV));

	// Create depth/stencil texture
	D3D11_TEXTURE2D_DESC _depthDesc = {};
	_depthDesc.Width = Width;
	_depthDesc.Height = Height;
	_depthDesc.Usage = D3D11_USAGE_DEFAULT;
	_depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	_depthDesc.SampleDesc = sd.SampleDesc;
	_depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	_depthDesc.ArraySize = 1;
	_depthDesc.MipLevels = 1;
	
	Microsoft::WRL::ComPtr<ID3D11Texture2D> _pDepthTexture;

	GFX_THROW_INFO(m_pDevice->CreateTexture2D(&_depthDesc, nullptr, &_pDepthTexture));
	GFX_THROW_INFO(m_pDevice->CreateDepthStencilView(_pDepthTexture.Get(), nullptr, &m_pDSV));

	D3D11_VIEWPORT vp = {};
	vp.Width = Width;
	vp.Height = Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	m_pImmediateContext->RSSetViewports(1, &vp);

	m_pImmediateContext->OMSetRenderTargets(1, m_pRTV.GetAddressOf(), m_pDSV.Get());
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
	float _color[] = { red, green, blue, 1.0f };
	m_pImmediateContext->ClearRenderTargetView(m_pRTV.Get(), _color);
	m_pImmediateContext->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Graphics::EndFrame()
{
	HRESULT hr;
#ifndef NDEBUG
	infoManager.Set();
#endif
	if (FAILED(hr = m_pSwapChain->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw GFX_DEVICE_REMOVED_EXCEPT(m_pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw GFX_EXCEPT(hr);
		}
	}
}

void Graphics::DrawIndexed(UINT count) noexcept(!IS_DEBUG)
{
	GFX_THROW_INFO_ONLY(m_pImmediateContext->DrawIndexed(count, 0u, 0u));
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
	m_Projection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return m_Projection;
}


Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
	: Exception(line, file), m_HR(hr)
{
	// Join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		m_Info += m;
		m_Info.push_back('\n');
	}
	// Remove final newline if exists
	if (!m_Info.empty())
		m_Info.pop_back();
}


const char* Graphics::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorString() << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl;
	if (!m_Info.empty())
		oss << "\n[Error Info]\n" << GetErrorInfo() << "\n\n";
	oss << GetOriginString();
	m_WhatBuffer = oss.str();
	return m_WhatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
	return "Chili Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return m_HR;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(m_HR);
}

std::string Graphics::HrException::GetErrorDescription() const noexcept
{
	static char _buffer[512];
	DXGetErrorDescription(m_HR, _buffer, sizeof(_buffer));
	return _buffer;
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
	return m_Info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return "Chili Graphics Exception [Device Removed] (DXGI_ERRRO_DEVICE_REMOVED)";
}

Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
	: Exception(line, file)
{
	// Join all info messages with newlines into single string
	for (const auto& m : infoMsgs)
	{
		m_Info += m;
		m_Info.push_back('\n');
	}
	// Remove final newline if exists
	if (!m_Info.empty())
		m_Info.pop_back();
}

const char* Graphics::InfoException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "\n[Error Info]\n" << GetErrorInfo() << "\n\n";
	oss << GetOriginString();
	m_WhatBuffer = oss.str();
	return m_WhatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
	return "Chili Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
	return m_Info;
}
