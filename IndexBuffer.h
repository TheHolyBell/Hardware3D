#pragma once
#include "Bindable.h"
#include <vector>

namespace Bind
{
	class IndexBuffer : public Bindable
	{
	public:
		IndexBuffer(Graphics& gfx, const std::vector<unsigned short>& indices);
		virtual void Bind(Graphics& gfx) noexcept override;
		UINT GetCount() const noexcept;
	protected:
		UINT m_Count;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
	};
}