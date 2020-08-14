#pragma once
#include "BufferResource.h"
#include "Bindable.h"

class Graphics;
class Surface;

namespace Bind
{
	class DepthStencil;

	class RenderTarget : public Bindable, public BufferResource
	{
	public:

		virtual void BindAsBuffer(Graphics& gfx) noxnd override;
		virtual void BindAsBuffer(Graphics& gfx, BufferResource* depthStencil) noxnd override;
		virtual void BindAsBuffer(Graphics& gfx, DepthStencil* depthStencil) noxnd;
		virtual void Clear(Graphics& gfx) noxnd override;
		virtual void Clear(Graphics& gfx, const std::array<float, 4>& color) noxnd;
		
		virtual void Resize(Graphics& gfx, UINT width, UINT height);

		void Reset(Graphics& gfx, ID3D11Texture2D* pTexture);
		void ReleaseBuffer();

		UINT GetWidth() const noexcept;
		UINT GetHeight() const noexcept;
	private:
		void BindAsBuffer(Graphics& gfx, ID3D11DepthStencilView* pDepthStencilView) noxnd;
	protected:
		RenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture);
		RenderTarget(Graphics& gfx, UINT width, UINT height);

	protected:
		UINT m_Width;
		UINT m_Height;

		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	};

	class ShaderInputRenderTarget : public RenderTarget
	{
	public:
		ShaderInputRenderTarget(Graphics& gfx, UINT width, UINT height, UINT slot);
		virtual void Bind(Graphics& gfx) noexcept override;

		virtual void Resize(Graphics& gfx, UINT width, UINT height) override;
	
		Surface ToSurface(Graphics& gfx) const;
	private:
		UINT m_Slot;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
	};

	// RT for Graphics to create RenderTarget for the back buffer
	class OutputOnlyRenderTarget : public RenderTarget
	{
		friend Graphics;
	public:
		virtual void Bind(Graphics& gfx) noexcept override;
	private:
		OutputOnlyRenderTarget(Graphics& gfx, ID3D11Texture2D* pTexture);
	};
}