#pragma once
#include "Bindable.h"
#include <array>
#include <memory>

namespace Bind
{
	class Rasterizer : public Bindable
	{
	public:
		Rasterizer(Graphics& gfx, bool twoSided);

		virtual void Bind(Graphics& gfx) noexcept override;
		virtual std::string GetUID() const noexcept override;

		static std::shared_ptr<Rasterizer> Resolve(Graphics& gfx, bool twoSided);
		static std::string GenerateUID(bool twoSided);
	protected:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRasterizerState;
		bool m_bTwoSided;
	};
}