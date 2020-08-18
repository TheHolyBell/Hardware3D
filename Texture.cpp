#include "Texture.h"
#include "Surface.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include "WICTextureLoader.h"

namespace Bind
{
	namespace wrl = Microsoft::WRL;

	Texture::Texture(Graphics& gfx, const std::string& path, UINT slot)
		:
		m_Path(path),
		m_Slot(slot)
	{
		INFOMAN(gfx);

		/*// load surface
		const auto s = Surface::FromFile(path);
		m_bHasAlpha = s.AlphaLoaded();

		// create texture resource
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = s.GetWidth();
		textureDesc.Height = s.GetHeight();
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = s.GetBufferPtr();
		sd.SysMemPitch = s.GetWidth() * sizeof(Surface::Color);
		wrl::ComPtr<ID3D11Texture2D> pTexture;
		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(
			&textureDesc, &sd, &pTexture
		));

		// create the resource view on the texture
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(
			pTexture.Get(), &srvDesc, &m_pTextureView
		));*/

		/*Microsoft::WRL::ComPtr<ID3D11Resource> _pTexture;

		GFX_THROW_INFO( DirectX::CreateWICTextureFromFile(GetDevice(gfx), GetContext(gfx), std::wstring(path.begin(), path.end()).c_str() ,&_pTexture, &m_pTextureView));
		//GetContext(gfx)->GenerateMips(m_pTextureView.Get());

		D3D11_SHADER_RESOURCE_VIEW_DESC _srvDesc = {};

		m_pTextureView->GetDesc(&_srvDesc);
		if (_srvDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM || _srvDesc.Format == DXGI_FORMAT_B8G8R8A8_UNORM)
			m_bHasAlpha = true;*/

		const auto s = Surface::FromFile(path);
		m_bHasAlpha = s.AlphaLoaded();

		// Create texture resource
		D3D11_TEXTURE2D_DESC _textureDesc = {};
		_textureDesc.Width = s.GetWidth();
		_textureDesc.Height = s.GetHeight();
		_textureDesc.MipLevels = 0;
		_textureDesc.ArraySize = 1;
		_textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		_textureDesc.SampleDesc.Count = 1;
		_textureDesc.SampleDesc.Quality = 0;
		_textureDesc.Usage = D3D11_USAGE_DEFAULT;
		_textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		_textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		wrl::ComPtr<ID3D11Texture2D> _pTexture;
		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&_textureDesc,
			nullptr, &_pTexture));

		// Write image data into top mip level
		GetContext(gfx)->UpdateSubresource(_pTexture.Get(),
			0, nullptr, s.GetBufferPtrConst(), s.GetWidth() * sizeof(Surface::Color), 0);
	
		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(_pTexture.Get(),
			nullptr, &m_pTextureView));

		// Generate the mip-chain using the GPU rendering pipeline
		GetContext(gfx)->GenerateMips(m_pTextureView.Get());
	}

	void Texture::Bind(Graphics& gfx) noxnd
	{
		GetContext(gfx)->PSSetShaderResources(m_Slot, 1u, m_pTextureView.GetAddressOf());
	}
	std::shared_ptr<Texture> Texture::Resolve(Graphics& gfx, const std::string& path, UINT slot)
	{
		return Codex::Resolve<Texture>(gfx, path, slot);
	}
	std::string Texture::GenerateUID(const std::string& path, UINT slot)
	{
		using namespace std::string_literals;
		return typeid(Texture).name() + "#"s + path + "#" + std::to_string(slot);
	}
	bool Texture::HasAlpha() const noexcept
	{
		return m_bHasAlpha;
	}
	UINT Texture::CalculateNumberOfMipLevels(UINT width, UINT height) noexcept
	{
		const float xSteps = std::ceil(log2((float)width));
		const float ySteps = std::ceil(log2((float)height));
		return (UINT)std::max(xSteps, ySteps);
	}
	std::string Texture::GetUID() const noexcept
	{
		return GenerateUID(m_Path, m_Slot);
	}
}