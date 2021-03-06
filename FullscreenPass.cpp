#include "FullscreenPass.h"
#include "BindableCommon.h"

namespace RenderGraph
{
	namespace dx = DirectX;

	FullscreenPass::FullscreenPass(const std::string& name, Graphics& gfx) noxnd
		: BindingPass(name)
	{
		// setup fullscreen geometry
		Dynamic::VertexLayout lay;
		lay.Append(Dynamic::VertexLayout::Position2D);
		Dynamic::VertexBuffer bufFull{ lay };
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1,-1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1,1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ 1,1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ 1,-1 });
		AddBind(Bind::VertexBuffer::Resolve(gfx, "$Full", std::move(bufFull)));
		std::vector<unsigned short> indices = { 0,1,2,0,2,3 };
		AddBind(Bind::IndexBuffer::Resolve(gfx, "$Full", std::move(indices)));
		// setup other common fullscreen bindables
		auto vs = Bind::VertexShader::Resolve(gfx, "Fullscreen_VS.cso");
		AddBind(Bind::InputLayout::Resolve(gfx, lay, *vs));
		AddBind(std::move(vs));
		AddBind(Bind::Topology::Resolve(gfx));
		AddBind(Bind::Rasterizer::Resolve(gfx, false));
	}

	void FullscreenPass::Execute(Graphics & gfx) const noxnd
	{
		BindAll(gfx);
		gfx.DrawIndexed(6u);
	}
}