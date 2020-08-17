#pragma once
#include "Bindable.h"
#include "BufferResource.h"

class Graphics;
class Surface;

namespace Bind
{
	class RenderTarget;

	class DepthStencil : public Bindable, public BufferResource
	{
		friend RenderTarget;
	public:
		enum class Usage
		{
			DepthStencil,
			ShadowDepth,
		};
	public:
		void BindAsBuffer(Graphics& gfx) noxnd override;
		void BindAsBuffer(Graphics& gfx, BufferResource* renderTarget) noxnd override;
		void BindAsBuffer(Graphics& gfx, RenderTarget* rt) noxnd;
		void Clear(Graphics& gfx) noxnd override;
		virtual void Resize(Graphics& gfx, UINT width, UINT height);
	
		Surface ToSurface(Graphics& gfx, bool linearlize = true) const;
		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
	protected:

		DepthStencil(Graphics& gfx, UINT width, UINT height, bool canBindShaderInput, Usage usage);
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
		UINT m_Width;
		UINT m_Height;
	};

		  
	class ShaderInputDepthStencil : public DepthStencil
	{
	public:
		ShaderInputDepthStencil(Graphics& gfx, UINT slot, Usage usage = Usage::DepthStencil);
		ShaderInputDepthStencil(Graphics& gfx, UINT width, UINT height, UINT slot, Usage usage = Usage::DepthStencil);
		virtual void Bind(Graphics& gfx) noexcept override;
		virtual void Resize(Graphics& gfx, UINT width, UINT height);
	private:
		UINT m_Slot;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
	};

	class OutputOnlyDepthStencil : public DepthStencil
	{
	public:
		OutputOnlyDepthStencil(Graphics& gfx);
		OutputOnlyDepthStencil(Graphics& gfx, UINT width, UINT height);
		virtual void Bind(Graphics& gfx) noexcept override;
	};
}