#include "Texture.h"
#include "Surface.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	Texture::Texture(Graphics& gfx, const Surface& s, UINT slot)
		: m_Slot(slot)
	{
		INFOMAN(gfx);

		// Create texture resource
		D3D11_TEXTURE2D_DESC _textureDesc = {};
		_textureDesc.Width = s.GetWidth();
		_textureDesc.Height = s.GetHeight();
		_textureDesc.MipLevels = 1;
		_textureDesc.ArraySize = 1;
		_textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		_textureDesc.SampleDesc.Count = 1;
		_textureDesc.SampleDesc.Quality = 0;
		_textureDesc.Usage = D3D11_USAGE_DEFAULT;
		_textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		D3D11_SUBRESOURCE_DATA _initData = {};
		_initData.pSysMem = s.GetBufferPtr();
		_initData.SysMemPitch = s.GetWidth() * sizeof(::Surface::Color);

		Microsoft::WRL::ComPtr<ID3D11Texture2D> _pTexture;
		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&_textureDesc,
			&_initData, &_pTexture));

		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(_pTexture.Get(),
			nullptr, &m_pTextureView));
	}

	void Texture::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetShaderResources(m_Slot, 1, m_pTextureView.GetAddressOf());
	}
}