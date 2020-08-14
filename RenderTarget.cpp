#include "RenderTarget.h"
#include "GraphicsThrowMacros.h"
#include "DepthStencil.h"
#include "Surface.h"
#include <array>

namespace wrl = Microsoft::WRL;

namespace Bind
{
	RenderTarget::RenderTarget(Graphics& gfx, UINT width, UINT height)
		: m_Width(width), m_Height(height)
	{
		INFOMAN(gfx);

		// Create texture resource
		D3D11_TEXTURE2D_DESC _textureDesc = {};
		_textureDesc.Width = width;
		_textureDesc.Height = height;
		_textureDesc.MipLevels = 1;
		_textureDesc.ArraySize = 1;
		_textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		_textureDesc.SampleDesc.Count = 1;
		_textureDesc.SampleDesc.Quality = 0;
		_textureDesc.Usage = D3D11_USAGE_DEFAULT;
		_textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		wrl::ComPtr<ID3D11Texture2D> _pTexture;
		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&_textureDesc, nullptr, &_pTexture));

		GFX_THROW_INFO(GetDevice(gfx)->CreateRenderTargetView(_pTexture.Get(),
			nullptr, &m_pRenderTargetView));
	}


	RenderTarget::RenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture)
	{
		INFOMAN(gfx);

		// Retrieve info from texture about its dimensions
		D3D11_TEXTURE2D_DESC _textureDesc;
		pTexture->GetDesc(&_textureDesc);
		m_Width = _textureDesc.Width;
		m_Height = _textureDesc.Height;

		GFX_THROW_INFO(GetDevice(gfx)->CreateRenderTargetView(pTexture,
			nullptr, &m_pRenderTargetView));
	}

	void RenderTarget::BindAsBuffer(Graphics& gfx) noxnd
	{
		ID3D11DepthStencilView* const _Null = nullptr;
		BindAsBuffer(gfx, _Null);
	}

	void RenderTarget::BindAsBuffer(Graphics& gfx, BufferResource* depthStencil) noxnd
	{
		assert(dynamic_cast<DepthStencil*>(depthStencil) != nullptr);
		BindAsBuffer(gfx, static_cast<DepthStencil*>(depthStencil));
	}

	void RenderTarget::BindAsBuffer(Graphics& gfx, DepthStencil* depthStencil) noxnd
	{
		BindAsBuffer(gfx, depthStencil != nullptr ? depthStencil->m_pDepthStencilView.Get() : nullptr);
	}

	void RenderTarget::BindAsBuffer(Graphics& gfx, ID3D11DepthStencilView* pDepthStencilView) noxnd
	{
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(GetContext(gfx)->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), pDepthStencilView));

		// Configure viewport
		D3D11_VIEWPORT vp = {};
		vp.Width = m_Width;
		vp.Height = m_Height;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;

		GFX_THROW_INFO_ONLY(GetContext(gfx)->RSSetViewports(1, &vp));
	}

	void RenderTarget::Clear(Graphics& gfx, const std::array<float, 4>& color) noxnd
	{
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(GetContext(gfx)->ClearRenderTargetView(m_pRenderTargetView.Get(), color.data()));
	}

	void RenderTarget::Clear(Graphics& gfx) noxnd
	{
		Clear(gfx, { 0.0f, 0.0f, 0.0f, 0.0f });
	}


	void Bind::RenderTarget::Resize(Graphics& gfx, UINT width, UINT height)
	{
		INFOMAN(gfx);
		m_Width = width;
		m_Height = height;

		Microsoft::WRL::ComPtr<ID3D11Resource> _pResource;
		m_pRenderTargetView->GetResource(&_pResource);

		ID3D11Texture2D* _pTexture = reinterpret_cast<ID3D11Texture2D*>(_pResource.Get());
		D3D11_TEXTURE2D_DESC _texDesc = {};
		_pTexture->GetDesc(&_texDesc);

		_texDesc.Width = width;
		_texDesc.Height = height;

		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&_texDesc, nullptr, &_pTexture));
		GFX_THROW_INFO(GetDevice(gfx)->CreateRenderTargetView(_pTexture, nullptr, &m_pRenderTargetView));
	}

	void Bind::RenderTarget::Reset(Graphics& gfx, ID3D11Texture2D* pTexture)
	{
		INFOMAN(gfx);

		D3D11_TEXTURE2D_DESC _texDesc = {};
		pTexture->GetDesc(&_texDesc);

		m_Width = _texDesc.Width;
		m_Height = _texDesc.Height;

		GFX_THROW_INFO(GetDevice(gfx)->CreateRenderTargetView(pTexture, nullptr, &m_pRenderTargetView));
	}

	void Bind::RenderTarget::ReleaseBuffer()
	{
		m_pRenderTargetView.Reset();
	}

	UINT RenderTarget::GetWidth() const noexcept
	{
		return m_Width;
	}

	UINT RenderTarget::GetHeight() const noexcept
	{
		return m_Height;
	}


	ShaderInputRenderTarget::ShaderInputRenderTarget(Graphics& gfx, UINT width, UINT height, UINT slot)
		: RenderTarget(gfx, width, height),
		m_Slot(slot)
	{
		INFOMAN(gfx);

		wrl::ComPtr<ID3D11Resource> _pResource;
		m_pRenderTargetView->GetResource(&_pResource);

		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
			_pResource.Get(), &srvDesc, &m_pShaderResourceView
		));
	}

	void ShaderInputRenderTarget::Bind(Graphics& gfx) noexcept
	{
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(m_Slot, 1, m_pShaderResourceView.GetAddressOf()));
	}

	void Bind::ShaderInputRenderTarget::Resize(Graphics& gfx, UINT width, UINT height)
	{
		INFOMAN(gfx);
		RenderTarget::Resize(gfx, width, height);

		Microsoft::WRL::ComPtr<ID3D11Resource> _pResource;
		m_pRenderTargetView->GetResource(&_pResource);

		ID3D11Texture2D* _pTexture = reinterpret_cast<ID3D11Texture2D*>(_pResource.Get());
		
		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(_pTexture, nullptr, &m_pShaderResourceView));
	}

	Surface Bind::ShaderInputRenderTarget::ToSurface(Graphics& gfx) const
	{
		INFOMAN(gfx);
		
		// Creating a temp texture compatible with the source, but with CPU read access
		Microsoft::WRL::ComPtr<ID3D11Resource> _pResSource;
		m_pShaderResourceView->GetResource(&_pResSource);
		wrl::ComPtr<ID3D11Texture2D> _pTexSource;
		_pResSource.As<ID3D11Texture2D>(&_pTexSource);

		D3D11_TEXTURE2D_DESC _textureDesc = {};
		_pTexSource->GetDesc(&_textureDesc);
		_textureDesc.Usage = D3D11_USAGE_STAGING;
		_textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		_textureDesc.BindFlags = 0;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> _pTexTemp;
		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&_textureDesc,
			nullptr, &_pTexTemp));

		// Copy texture contents
		GFX_THROW_INFO_ONLY(GetContext(gfx)->CopyResource(_pTexTemp.Get(), _pTexSource.Get()));
	
		// Create surface and copy from temp texture to it
		const auto width = GetWidth();
		const auto height = GetHeight();
		Surface _Surface{ width, height };
		D3D11_MAPPED_SUBRESOURCE msr = {};
		GFX_THROW_INFO(GetContext(gfx)->Map(_pTexTemp.Get(), 0, D3D11_MAP_READ, 0, &msr));
		auto pSrcBytes = static_cast<const char*>(msr.pData);
		for (unsigned int y = 0; y < height; y++)
		{
			auto pSrcRow = reinterpret_cast<const Surface::Color*>(pSrcBytes + msr.RowPitch * size_t(y));
			for (unsigned int x = 0; x < width; x++)
			{
				_Surface.PutPixel(x, y, *(pSrcRow + x));
			}
		}
		GFX_THROW_INFO_ONLY(GetContext(gfx)->Unmap(_pTexTemp.Get(), 0));

		return _Surface;
	}


	void OutputOnlyRenderTarget::Bind(Graphics& gfx) noexcept
	{
		assert("Cannot bind OutputOnlyRenderTarget as shader input" && false);
	}

	OutputOnlyRenderTarget::OutputOnlyRenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture)
		: RenderTarget(gfx, pTexture)
	{}
}