#pragma once

#include "Bindable.h"
#include <memory>

namespace Bind
{
	class Sampler : public Bindable
	{
	public:
		Sampler(Graphics& gfx);

		virtual void Bind(Graphics& gfx) noexcept override;
		virtual std::string GetUID() const noexcept override;

		static std::string GenerateUID();
		static std::shared_ptr<Sampler> Resolve(Graphics& gfx);
	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
	};
}