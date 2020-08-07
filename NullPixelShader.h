#pragma once
#include "Bindable.h"
#include <memory>

namespace Bind
{
	class NullPixelShader : public Bindable
	{
	public:
		NullPixelShader(Graphics& gfx);

		virtual void Bind(Graphics& gfx) noexcept override;
		virtual std::string GetUID() const noexcept override;

		static std::shared_ptr<NullPixelShader> Resolve(Graphics& gfx);
		static std::string GenerateUID();
	};
}