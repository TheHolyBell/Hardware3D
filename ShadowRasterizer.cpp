#include "ShadowRasterizer.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"

namespace Bind
{
	ShadowRasterizer::ShadowRasterizer(Graphics& gfx, int depthBias, float slopeBias, float clamp)
	{
		ChangeDepthBiasParameters(gfx, depthBias, slopeBias, clamp);
	}
	void ShadowRasterizer::ChangeDepthBiasParameters(Graphics& gfx, int depthBias, float slopeBias, float clamp)
	{
		m_DepthBias = depthBias;
		m_SlopeBias = slopeBias;
		m_Clamp = clamp;

		INFOMAN(gfx);

		D3D11_RASTERIZER_DESC _rasterizerDesc = CD3D11_RASTERIZER_DESC{ CD3D11_DEFAULT{} };
		_rasterizerDesc.DepthBias = depthBias;
		_rasterizerDesc.SlopeScaledDepthBias = slopeBias;
		_rasterizerDesc.DepthBiasClamp = clamp;

		GFX_THROW_INFO(GetDevice(gfx)->CreateRasterizerState(&_rasterizerDesc, &m_pRasterizerState));
	}
	void ShadowRasterizer::Bind(Graphics& gfx) noxnd
	{
		INFOMAN_NOHR(gfx);
		GFX_THROW_INFO_ONLY(GetContext(gfx)->RSSetState(m_pRasterizerState.Get()));
	}
	int ShadowRasterizer::GetDepthBias() const
	{
		return m_DepthBias;
	}
	float ShadowRasterizer::GetSlopeBias() const
	{
		return m_SlopeBias;
	}
	float ShadowRasterizer::GetClamp() const
	{
		return m_Clamp;;
	}
}