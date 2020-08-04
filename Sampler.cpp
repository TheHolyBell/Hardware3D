#include "Sampler.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

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
		_sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&_sampDesc, &m_pSampler));
	}

	void Sampler::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetSamplers(0, 1, m_pSampler.GetAddressOf());
	}
	std::string Sampler::GetUID() const noexcept
	{
		return GenerateUID();
	}
	std::string Sampler::GenerateUID()
	{
		return typeid(Sampler).name();
	}
	std::shared_ptr<Sampler> Sampler::Resolve(Graphics& gfx)
	{
		return Codex::Resolve<Sampler>(gfx);
	}
}