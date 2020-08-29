#include "ShadowSampler.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	ShadowSampler::ShadowSampler(Graphics& gfx)
	{
		for (size_t i = 0; i < 4; ++i)
		{
			m_CurrentSampler = i;
			m_pSamplerStates[i] = MakeSampler(gfx, GetBilinear(), GetHwPcf());
		}
		SetBilinear(true);
		SetHwPcf(true);
	}

	void ShadowSampler::SetBilinear(bool bilinear)
	{
		m_CurrentSampler = (m_CurrentSampler & ~0b01) | (bilinear ? 0b01 : 0);
	}

	void ShadowSampler::SetHwPcf(bool hwPcf)
	{
		m_CurrentSampler = (m_CurrentSampler & ~0b10) | (hwPcf ? 0b10 : 0);

	}

	bool ShadowSampler::GetBilinear() const
	{
		return m_CurrentSampler & 0b01;
	}

	bool ShadowSampler::GetHwPcf() const
	{
		return m_CurrentSampler & 0b10;
	}

	UINT ShadowSampler::GetCurrentSlot() const
	{
		return GetHwPcf() ? 1 : 2;
	}

	size_t ShadowSampler::ShadowSamplerIndex(bool bilinear, bool hwPcf)
	{
		return (bilinear ? 0b01 : 0) + (hwPcf ? 0b10 : 0);
	}

	Microsoft::WRL::ComPtr<ID3D11SamplerState> ShadowSampler::MakeSampler(Graphics& gfx, bool bilinear, bool hwPcf)
	{
		INFOMAN(gfx);

		D3D11_SAMPLER_DESC _samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };

		_samplerDesc.BorderColor[0] = 1.0f;
		_samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		_samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;

		if (hwPcf)
		{
			_samplerDesc.Filter = bilinear ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR : D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
			_samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		}
		else
		{
			_samplerDesc.Filter = bilinear ? D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_POINT;
		}

		Microsoft::WRL::ComPtr<ID3D11SamplerState> _pSampler;
		GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&_samplerDesc, &_pSampler));
		return std::move(_pSampler);
	}

	void ShadowSampler::Bind(Graphics& gfx) noxnd
	{
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetSamplers(GetCurrentSlot(), 1, m_pSamplerStates[m_CurrentSampler].GetAddressOf()));
	}
}
