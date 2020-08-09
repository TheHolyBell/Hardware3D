#pragma once
#include "Graphics.h"
#include "GraphicsResource.h"

class DepthStencil : public GraphicsResource
{
	friend class RenderTarget;
	friend class Graphics;
public:
	DepthStencil(Graphics& gfx, UINT width, UINT height);
	void BindAsDepthStencil(Graphics& gfx) const noexcept;
	void Clear(Graphics& gfx) const noexcept;
	ID3D11DepthStencilView* GetView() const noexcept;

	void Resize(Graphics& gfx, int Width, int Height);
private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
};