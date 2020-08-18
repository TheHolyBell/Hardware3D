#pragma once
#include "Bindable.h"
#include <array>
#include <optional>
#include <memory>

namespace Bind
{
	class Blender : public Bindable
	{
	public:
		Blender(Graphics& gfx, bool blending, std::optional<float> factor = {});
		
		virtual void Bind(Graphics& gfx) noxnd override;
		virtual std::string GetUID() const noexcept override;

		void SetFactor(float factor) noxnd;
		float GetFactor() const noxnd;

		static std::shared_ptr<Blender> Resolve(Graphics& gfx, bool blending, std::optional<float> factor = {});
		static std::string GenerateUID(bool blending, std::optional<float> factor);
	protected:
		Microsoft::WRL::ComPtr<ID3D11BlendState> m_pBlendState;
		bool m_bBlending;
		std::optional<std::array<float, 4>> m_Factors;
	};
}