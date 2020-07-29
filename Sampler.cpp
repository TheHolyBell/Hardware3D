#include "Sampler.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	Sampler::Sampler(Graphics& gfx)
	{
		INFOMAN(gfx);

		D3D11_SAMPLER_DESC _sampDesc = {};
		_sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		_sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		_sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		_sampDesc.MaxAnisotropy = 16;
		_sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;

		GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&_sampDesc, &m_pSampler));
	}

	void Sampler::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetSamplers(0, 1, m_pSampler.GetAddressOf());
	}
}