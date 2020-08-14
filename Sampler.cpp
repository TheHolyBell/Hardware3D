#include "Sampler.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	Sampler::Sampler(Graphics& gfx, Type type, bool reflect)
		:
		m_Type(type),
		m_bReflect(reflect)
	{
		INFOMAN(gfx);

		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.Filter = [type]() {
			switch (type)
			{
			case Type::Anisotropic: return D3D11_FILTER_ANISOTROPIC;
			case Type::Point: return D3D11_FILTER_MIN_MAG_MIP_POINT;
			default:
			case Type::Bilinear: return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			}
		}();
		samplerDesc.AddressU = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&samplerDesc, &m_pSampler));
	}

	void Sampler::Bind(Graphics& gfx) noexcept
	{
		INFOMAN_NOHR(gfx);
		GetContext(gfx)->PSSetSamplers(0, 1, m_pSampler.GetAddressOf());
	}
	std::string Sampler::GetUID() const noexcept
	{
		return GenerateUID(m_Type, m_bReflect);
	}
	std::string Sampler::GenerateUID(Type type, bool reflect)
	{
		using namespace std::string_literals;
		return typeid(Sampler).name() + "#"s + std::to_string((int)type) + (reflect ? "R"s : "W"s);
	}
	std::shared_ptr<Sampler> Sampler::Resolve(Graphics& gfx, Type type, bool reflect)
	{
		return Codex::Resolve<Sampler>(gfx, type, reflect);
	}
}