#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include "DynamicConstant.h"
#include "TechniqueProbe.h"

namespace Bind
{
	class ConstantBufferEx : public Bindable
	{
	public:
		void Update(Graphics& gfx, const Dynamic::Buffer& buf)
		{
			assert(&buf.GetRootLayoutElement() == &GetRootLayoutElement());
			INFOMAN(gfx);

			D3D11_MAPPED_SUBRESOURCE msr;
			GFX_THROW_INFO(GetContext(gfx)->Map(
				m_pConstantBuffer.Get(), 0u,
				D3D11_MAP_WRITE_DISCARD, 0u,
				&msr
			));
			memcpy(msr.pData, buf.GetData(), buf.GetSizeInBytes());
			GetContext(gfx)->Unmap(m_pConstantBuffer.Get(), 0u);
		}
		// this exists for validation of the update buffer layout
		// reason why it's not getbuffer is becasue nocache doesn't store buffer
		virtual const Dynamic::LayoutElement& GetRootLayoutElement() const noexcept = 0;
	protected:
		ConstantBufferEx(Graphics& gfx, const Dynamic::LayoutElement& layoutRoot, UINT slot, const Dynamic::Buffer* pBuf)
			:
			m_Slot(slot)
		{
			INFOMAN(gfx);

			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = (UINT)layoutRoot.GetSizeInBytes();
			cbd.StructureByteStride = 0u;

			if (pBuf != nullptr)
			{
				D3D11_SUBRESOURCE_DATA csd = {};
				csd.pSysMem = pBuf->GetData();
				GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &m_pConstantBuffer));
			}
			else
			{
				GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &m_pConstantBuffer));
			}
		}
	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBuffer;
		UINT m_Slot;
	};

	class PixelConstantBufferEx : public ConstantBufferEx
	{
	public:
		using ConstantBufferEx::ConstantBufferEx;
		void Bind(Graphics& gfx) noexcept override
		{
			GetContext(gfx)->PSSetConstantBuffers(m_Slot, 1u, m_pConstantBuffer.GetAddressOf());
		}
	};

	class VertexConstantBufferEx : public ConstantBufferEx
	{
	public:
		using ConstantBufferEx::ConstantBufferEx;
		void Bind(Graphics& gfx) noexcept override
		{
			GetContext(gfx)->VSSetConstantBuffers(m_Slot, 1u, m_pConstantBuffer.GetAddressOf());
		}
	};

	template<class T>
	class CachingConstantBufferEx : public T
	{
	public:
		CachingConstantBufferEx(Graphics& gfx, const Dynamic::CookedLayout& layout, UINT slot)
			:
			T(gfx, *layout.ShareRoot(), slot, nullptr),
			m_Buffer(Dynamic::Buffer(layout))
		{}
		CachingConstantBufferEx(Graphics& gfx, const Dynamic::Buffer& buf, UINT slot)
			:
			T(gfx, buf.GetRootLayoutElement(), slot, &buf),
			m_Buffer(buf)
		{}
		virtual const Dynamic::LayoutElement& GetRootLayoutElement() const noexcept override
		{
			return m_Buffer.GetRootLayoutElement();
		}
		const Dynamic::Buffer& GetBuffer() const noexcept
		{
			return m_Buffer;
		}
		void SetBuffer(const Dynamic::Buffer& buf_in)
		{
			m_Buffer.CopyFrom(buf_in);
			m_bDirty = true;
		}
		virtual void Bind(Graphics& gfx) noexcept override
		{
			if (m_bDirty)
			{
				T::Update(gfx, m_Buffer);
				m_bDirty = false;
			}
			T::Bind(gfx);
		}
		virtual void Accept(TechniqueProbe& probe) override
		{
			if (probe.VisitBuffer(m_Buffer))
			{
				m_bDirty = true;
			}
		}
	private:
		bool m_bDirty = false;
		Dynamic::Buffer m_Buffer;
	};

	using CachingPixelConstantBufferEx = CachingConstantBufferEx<PixelConstantBufferEx>;
	using CachingVertexConstantBufferEx = CachingConstantBufferEx<VertexConstantBufferEx>;

	/*class NocachePixelConstantBufferEX : public PixelConstantBufferEX
	{
	public:
		NocachePixelConstantBufferEX(Graphics& gfx, const Dynamic::CookedLayout& layout, UINT slot)
			:
			PixelConstantBufferEX(gfx, *layout.ShareRoot(), slot, nullptr),
			m_pLayoutRoot(layout.ShareRoot())
		{

		}
		NocachePixelConstantBufferEX(Graphics& gfx, const Dynamic::Buffer& buf, UINT slot)
			: PixelConstantBufferEX(gfx, buf.GetRootLayoutElement(), slot, &buf),
			m_pLayoutRoot(buf.ShareLayoutRoot())
		{}
		const Dynamic::LayoutElement& GetRootLayoutElement() const noexcept override
		{
			return *m_pLayoutRoot;
		}
	private:
		std::shared_ptr<Dynamic::LayoutElement> m_pLayoutRoot;
	};*/
}