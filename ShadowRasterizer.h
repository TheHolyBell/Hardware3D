#pragma once
#include "Bindable.h"
#include <array>

namespace Bind
{
	class ShadowRasterizer : public Bindable
	{
	public:
		ShadowRasterizer(Graphics& gfx, int depthBias, float slopeBias, float clamp);
		void ChangeDepthBiasParameters(Graphics& gfx, int depthBias, float slopeBias, float clamp);

		virtual void Bind(Graphics& gfx) noxnd override;
		int GetDepthBias() const;
		float GetSlopeBias() const;
		float GetClamp() const;
	protected:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizerState;
		int m_DepthBias;
		float m_SlopeBias;
		float m_Clamp;
	};
}