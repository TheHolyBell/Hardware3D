#include "RenderTarget.h"
#include "GraphicsThrowMacros.h"
#include "DepthStencil.h"

RenderTarget::RenderTarget(Graphics& gfx, UINT width, UINT height)
{
	Resize(gfx, width, height);
}

void RenderTarget::Clear(Graphics& gfx) const noexcept
{
	Clear(gfx, { 0.0f, 0.0f, 0.0f, 0.0f });
}

void RenderTarget::Clear(Graphics& gfx, const std::array<float, 4>& color) const noexcept
{
	GetContext(gfx)->ClearRenderTargetView(m_pTargetView.Get(), color.data());
}

void RenderTarget::Resize(Graphics& gfx, int Width, int Height)
{
	m_Width = Width;
	m_Height = Height;

	INFOMAN(gfx);

	// Create texture resource
	D3D11_TEXTURE2D_DESC _textureDesc = {};
	_textureDesc.Width = Width;
	_textureDesc.Height = Height;
	_textureDesc.MipLevels = 1;
	_textureDesc.ArraySize = 1;
	_textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	_textureDesc.SampleDesc.Count = 1;
	_textureDesc.SampleDesc.Quality = 0;
	_textureDesc.Usage = D3D11_USAGE_DEFAULT;
	_textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> _pTexture;
	GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&_textureDesc, nullptr,
		&_pTexture));

	GFX_THROW_INFO(GetDevice(gfx)->CreateRenderTargetView(_pTexture.Get(), nullptr, &m_pTargetView));
	GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(_pTexture.Get(), nullptr, &m_pTextureView));
}

void RenderTarget::BindAsTexture(Graphics& gfx, UINT slot) const noexcept
{
	GetContext(gfx)->PSSetShaderResources(slot, 1, m_pTextureView.GetAddressOf());
}

void RenderTarget::BindAsTarget(Graphics& gfx) const noexcept
{
	D3D11_VIEWPORT vp = {};
	vp.Width = m_Width;
	vp.Height = m_Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	GetContext(gfx)->RSSetViewports(1, &vp);
	GetContext(gfx)->OMSetRenderTargets(1, m_pTargetView.GetAddressOf(), nullptr);
}

void RenderTarget::BindAsTarget(Graphics& gfx, const DepthStencil& depthStencil) const noexcept
{
	D3D11_VIEWPORT vp = {};
	vp.Width = m_Width;
	vp.Height = m_Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	GetContext(gfx)->RSSetViewports(1, &vp);
	GetContext(gfx)->OMSetRenderTargets(1, m_pTargetView.GetAddressOf(), depthStencil.m_pDepthStencilView.Get());
}
