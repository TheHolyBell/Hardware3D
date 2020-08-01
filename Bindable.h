#pragma once

#include "Graphics.h"
#include "ConditionalNoexcept.h"

namespace Bind
{
	class Bindable
	{
	public:
		virtual void Bind(Graphics& gfx) noexcept = 0;
		virtual std::string GetUID() const noexcept
		{
			assert(false && "This class didn't override GetUID() method");
			return "You fucked up";
		}
		virtual ~Bindable() = default;
	protected:
		static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
		static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
		static DxgiInfoManager& GetInfoManager(Graphics& gfx);
	};
}