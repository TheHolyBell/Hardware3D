#include "DepthStencil.h"
#include "GraphicsThrowMacros.h"

namespace wrl = Microsoft::WRL;

DepthStencil::DepthStencil(Graphics& gfx, UINT width, UINT height)
{
	Resize(gfx, width, height);
}

void DepthStencil::BindAsDepthStencil(Graphics& gfx) const noexcept
{
	GetContext(gfx)->OMSetRenderTargets(0, nullptr, m_pDepthStencilView.Get());
}

void DepthStencil::Clear(Graphics& gfx) const noexcept
{
	GetContext(gfx)->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0);
}

ID3D11DepthStencilView* DepthStencil::GetView() const noexcept
{
	return m_pDepthStencilView.Get();
}

void DepthStencil::Resize(Graphics& gfx, int Width, int Height)
{
	INFOMAN(gfx);

	// Create depth stencil texture
	Microsoft::WRL::ComPtr<ID3D11Texture2D> _pDepthStencil;
	D3D11_TEXTURE2D_DESC _depthDesc = {};
	_depthDesc.Width = Width;
	_depthDesc.Height = Height;
	_depthDesc.MipLevels = 1;
	_depthDesc.ArraySize = 1;
	_depthDesc.SampleDesc.Count = 1;
	_depthDesc.SampleDesc.Quality = 0;
	_depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	_depthDesc.Usage = D3D11_USAGE_DEFAULT;
	_depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&_depthDesc, nullptr, &_pDepthStencil));

	// Create target view of depth stencil texture
	GFX_THROW_INFO(GetDevice(gfx)->CreateDepthStencilView(_pDepthStencil.Get(),
		nullptr, &m_pDepthStencilView));
}
