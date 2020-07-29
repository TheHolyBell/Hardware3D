#pragma once

#include "Bindable.h"

namespace Bind
{
	class Sampler : public Bindable
	{
	public:
		Sampler(Graphics& gfx);
		virtual void Bind(Graphics& gfx) noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
	};
}