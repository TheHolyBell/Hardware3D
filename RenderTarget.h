#pragma once
#include "Graphics.h"
#include "GraphicsResource.h"
#include <array>

class DepthStencil;

class RenderTarget : public GraphicsResource
{
public:
	RenderTarget(Graphics& gfx, UINT width, UINT height);

	void Clear(Graphics& gfx) const noexcept;
	void Clear(Graphics& gfx, const std::array<float, 4>& color) const noexcept;

	void Resize(Graphics& gfx, int Width, int Height);

	void BindAsTexture(Graphics& gfx, UINT slot) const noexcept;
	void BindAsTarget(Graphics& gfx) const noexcept;
	void BindAsTarget(Graphics& gfx, const DepthStencil& depthStencil) const noexcept;
private:
	UINT m_Width;
	UINT m_Height;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pTargetView;
};