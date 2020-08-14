#include "DepthStencil.h"
#include "RenderTarget.h"
#include "Graphics.h"
#include "GraphicsThrowMacros.h"

namespace wrl = Microsoft::WRL;

namespace Bind
{
	void DepthStencil::Resize(Graphics& gfx, UINT width, UINT height)
	{
		INFOMAN(gfx);
		Microsoft::WRL::ComPtr<ID3D11Resource> _pBuffer;
		D3D11_TEXTURE2D_DESC _depthDesc = {};
		m_pDepthStencilView->GetResource(&_pBuffer);

		ID3D11Texture2D* _pTexture = reinterpret_cast<ID3D11Texture2D*>(_pBuffer.Get());
		_pTexture->GetDesc(&_depthDesc);

		_depthDesc.Width = width;
		_depthDesc.Width = height;

		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&_depthDesc, nullptr, &_pTexture));
		GFX_THROW_INFO(GetDevice(gfx)->CreateDepthStencilView(_pTexture, nullptr, &m_pDepthStencilView));
	}
	DepthStencil::DepthStencil(Graphics& gfx, UINT width, UINT height, bool canBindShaderInput)
	{
		INFOMAN(gfx);

		// create depth stensil texture
		wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
		D3D11_TEXTURE2D_DESC descDepth = {};
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1u;
		descDepth.ArraySize = 1u;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // this will need to be fixed
		descDepth.SampleDesc.Count = 1u;
		descDepth.SampleDesc.Quality = 0u;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | (canBindShaderInput ? D3D11_BIND_SHADER_RESOURCE : 0);
		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

		// create target view of depth stensil texture
		GFX_THROW_INFO(GetDevice(gfx)->CreateDepthStencilView(
			pDepthStencil.Get(), nullptr, &m_pDepthStencilView // nullptr will need to be replaced
		));
	}

	void DepthStencil::BindAsBuffer(Graphics& gfx) noxnd
	{
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(GetContext(gfx)->OMSetRenderTargets(0, nullptr, m_pDepthStencilView.Get()));
	}

	void DepthStencil::BindAsBuffer(Graphics& gfx, BufferResource* renderTarget) noxnd
	{
		assert(dynamic_cast<RenderTarget*>(renderTarget) != nullptr);
		BindAsBuffer(gfx, static_cast<RenderTarget*>(renderTarget));
	}

	void DepthStencil::BindAsBuffer(Graphics& gfx, RenderTarget* rt) noxnd
	{
		rt->BindAsBuffer(gfx, this);
	}

	void DepthStencil::Clear(Graphics& gfx) noxnd
	{
		GetContext(gfx)->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
	}


	ShaderInputDepthStencil::ShaderInputDepthStencil(Graphics& gfx, UINT slot)
		:
		ShaderInputDepthStencil(gfx, gfx.GetWidth(), gfx.GetHeight(), slot)
	{}

	ShaderInputDepthStencil::ShaderInputDepthStencil(Graphics& gfx, UINT width, UINT height, UINT slot)
		:
		DepthStencil(gfx, width, height, true),
		m_Slot(slot)
	{
		INFOMAN(gfx);

		wrl::ComPtr<ID3D11Resource> pRes;
		m_pDepthStencilView->GetResource(&pRes);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // this will need to be fixed
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
			pRes.Get(), &srvDesc, &m_pShaderResourceView
		));
	}

	void ShaderInputDepthStencil::Bind(Graphics& gfx) noexcept
	{
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(m_Slot, 1u, m_pShaderResourceView.GetAddressOf()));
	}

	void Bind::ShaderInputDepthStencil::Resize(Graphics& gfx, UINT width, UINT height)
	{
		DepthStencil::Resize(gfx, width, height);
		INFOMAN(gfx);

		Microsoft::WRL::ComPtr<ID3D11Resource> _pBuffer;
		D3D11_TEXTURE2D_DESC _depthDesc = {};
		m_pDepthStencilView->GetResource(&_pBuffer);

		ID3D11Texture2D* _pTexture = reinterpret_cast<ID3D11Texture2D*>(_pBuffer.Get());
		_pTexture->GetDesc(&_depthDesc);

		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(_pTexture, nullptr, &m_pShaderResourceView));
	}


	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& gfx)
		:
		OutputOnlyDepthStencil(gfx, gfx.GetWidth(), gfx.GetHeight())
	{}

	OutputOnlyDepthStencil::OutputOnlyDepthStencil(Graphics& gfx, UINT width, UINT height)
		:
		DepthStencil(gfx, width, height, false)
	{}

	void OutputOnlyDepthStencil::Bind(Graphics& gfx) noexcept
	{
		assert("OutputOnlyDepthStencil cannot be bound as shader input" && false);
	}
}