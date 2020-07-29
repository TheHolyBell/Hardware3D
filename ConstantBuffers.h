#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include <memory.h>

namespace Bind
{
	template<typename C>
	class ConstantBuffer : public Bindable
	{
	public:
		void Update(Graphics& gfx, const C& consts)
		{
			INFOMAN(gfx);

			D3D11_MAPPED_SUBRESOURCE msr = {};
			GFX_THROW_INFO(GetContext(gfx)->Map(
				m_pConstantBuffer.Get(), 0u,
				D3D11_MAP_WRITE_DISCARD, 0u,
				&msr
			));
			memcpy(msr.pData, &consts, sizeof(consts));
			GetContext(gfx)->Unmap(m_pConstantBuffer.Get(), 0u);
		}
		ConstantBuffer(Graphics& gfx, const C& consts, UINT slot = 0u)
			: m_Slot(slot)
		{
			INFOMAN(gfx);

			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(consts);
			cbd.StructureByteStride = 0u;

			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = &consts;
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &m_pConstantBuffer));
		}
		ConstantBuffer(Graphics& gfx, UINT slot = 0u)
			: m_Slot(slot)
		{
			INFOMAN(gfx);

			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(C);
			cbd.StructureByteStride = 0u;
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &m_pConstantBuffer));
		}
	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBuffer;
		UINT m_Slot;
	};

	template<typename C>
	class VertexConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::m_pConstantBuffer;
		using ConstantBuffer<C>::m_Slot;
		using Bindable::GetContext;
	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void Bind(Graphics& gfx) noexcept override
		{
			GetContext(gfx)->VSSetConstantBuffers(m_Slot, 1u, m_pConstantBuffer.GetAddressOf());
		}
	};

	template<typename C>
	class PixelConstantBuffer : public ConstantBuffer<C>
	{
		using ConstantBuffer<C>::m_pConstantBuffer;
		using ConstantBuffer<C>::m_Slot;
		using Bindable::GetContext;
	public:
		using ConstantBuffer<C>::ConstantBuffer;
		virtual void Bind(Graphics& gfx) noexcept override
		{
			GetContext(gfx)->PSSetConstantBuffers(m_Slot, 1, m_pConstantBuffer.GetAddressOf());
		}
	};
}