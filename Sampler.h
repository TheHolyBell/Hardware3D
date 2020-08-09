#pragma once

#include "Bindable.h"
#include <memory>

namespace Bind
{
	class Sampler : public Bindable
	{
	public:
		Sampler(Graphics& gfx, bool anistropicEnabled, bool reflect);

		virtual void Bind(Graphics& gfx) noexcept override;
		virtual std::string GetUID() const noexcept override;

		static std::string GenerateUID(bool anisotropicEnabled, bool reflect);
		static std::shared_ptr<Sampler> Resolve(Graphics& gfx, bool anistropicEnabled = true, bool reflect = false);
	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
		bool m_bAnisotropicEnabled;
		bool m_bReflect;
	};
}