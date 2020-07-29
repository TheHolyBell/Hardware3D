#pragma once
#include "Bindable.h"
#include "Surface.h"

namespace Bind
{
	class Texture : public Bindable
	{
	public:
		Texture(Graphics& gfx, const Surface& s, UINT slot = 0u);
		virtual void Bind(Graphics& gfx) noexcept override;
	protected:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
		UINT m_Slot;
	};
}