#pragma once

#include "Bindable.h"
#include <memory>

namespace Bind
{
	class Sampler : public Bindable
	{
	public:
		enum class Type
		{
			Anisotropic,
			Bilinear,
			Point,
		};
	public:
		Sampler(Graphics& gfx, Type type, bool reflect);
		virtual void Bind(Graphics& gfx) noxnd override;
		static std::shared_ptr<Sampler> Resolve(Graphics& gfx, Type type = Type::Anisotropic, bool reflect = false);
		static std::string GenerateUID(Type type, bool reflect);
		virtual std::string GetUID() const noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
		Type m_Type;
		bool m_bReflect;
	};
}