#include "Sampler.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	Sampler::Sampler(Graphics& gfx, bool anisotropicEnabled, bool reflect)
		: m_bAnisotropicEnabled(anisotropicEnabled), m_bReflect(reflect)
	{
		INFOMAN(gfx);

		D3D11_SAMPLER_DESC _sampDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		_sampDesc.Filter = anisotropicEnabled ? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_MIN_MAG_MIP_POINT;
		_sampDesc.AddressU = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		_sampDesc.AddressV = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		_sampDesc.AddressW = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		_sampDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		_sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&_sampDesc, &m_pSampler));
	}

	void Sampler::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetSamplers(0, 1, m_pSampler.GetAddressOf());
	}
	std::string Sampler::GetUID() const noexcept
	{
		return GenerateUID(m_bAnisotropicEnabled, m_bReflect);
	}
	std::string Sampler::GenerateUID(bool anisotropicEnabled, bool reflect)
	{
		using namespace std::string_literals;
		return typeid(Sampler).name() + "#"s + (anisotropicEnabled ? "A"s : "a"s) + (reflect ? "R"s : "W"s);
	}
	std::shared_ptr<Sampler> Sampler::Resolve(Graphics& gfx, bool anisotropicEnabled, bool reflect)
	{
		return Codex::Resolve<Sampler>(gfx, anisotropicEnabled, reflect);
	}
}