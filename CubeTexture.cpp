#include "CubeTexture.h"
#include "WICTextureLoader.h"
#include "ChiliUtil.h"
#include "GraphicsThrowMacros.h"

#include "Surface.h"

namespace Bind
{
	CubeTexture::CubeTexture(Graphics& gfx, const std::string& path, UINT slot)
		: m_Path(path), m_Slot(slot)
	{
		INFOMAN(gfx);

		// Load 6 surfaces for cube faces
		std::vector<Surface> _Surfaces;
		for (int i = 0; i < 6; ++i)
			_Surfaces.push_back(Surface::FromFile(path + "\\" + std::to_string(i) + ".png"));
	
		// Texture description
		D3D11_TEXTURE2D_DESC _textureDesc = {};
		_textureDesc.Width = _Surfaces[0].GetWidth();
		_textureDesc.Height = _Surfaces[0].GetHeight();
		_textureDesc.MipLevels = 1;
		_textureDesc.ArraySize = 6;
		_textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		_textureDesc.SampleDesc.Count = 1;
		_textureDesc.SampleDesc.Quality = 0;
		_textureDesc.Usage = D3D11_USAGE_DEFAULT;
		_textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		_textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		// Subresource data
		D3D11_SUBRESOURCE_DATA _Data[6] = {};
		for (int i = 0; i < 6; ++i)
		{
			_Data[i].pSysMem = _Surfaces[i].GetBufferPtrConst();
			_Data[i].SysMemPitch = _Surfaces[i].GetBytePitch();
		}

		// Create the texture resource
		Microsoft::WRL::ComPtr<ID3D11Texture2D> _pTexture;
		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
			&_textureDesc, _Data, &_pTexture
		));

		// Create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC _srvDesc = {};
		_srvDesc.Format = _textureDesc.Format;
		_srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		_srvDesc.Texture2D.MipLevels = 1;
		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
			_pTexture.Get(), &_srvDesc, &m_pTextureView
		));
	}
	void CubeTexture::Bind(Graphics& gfx) noxnd
	{
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetShaderResources(m_Slot, 1, m_pTextureView.GetAddressOf()));
	}
}
