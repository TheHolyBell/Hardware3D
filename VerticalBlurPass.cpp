#include "VerticalBlurPass.h"
#include "Sink.h"
#include "Source.h"
#include "PixelShader.h"
#include "Blender.h"
#include "Stencil.h"
#include "Sampler.h"
#include "DepthStencil.h"

using namespace Bind;

namespace RenderGraph
{
	VerticalBlurPass::VerticalBlurPass(const std::string& name, Graphics& gfx)
		: FullscreenPass(name, gfx)
	{
		AddBind(PixelShader::Resolve(gfx, "BlurOutline_PS.cso"));
		AddBind(Blender::Resolve(gfx, true));
		AddBind(Stencil::Resolve(gfx, Stencil::Mode::Mask));
		AddBind(Sampler::Resolve(gfx, Sampler::Type::Bilinear, true));

		AddBindSink<RenderTarget>("scratchIn");
		AddBindSink<CachingPixelConstantBufferEx>("kernel");
		RegisterSink(DirectBindableSink<CachingPixelConstantBufferEx>::Make("direction", m_Direction));
		RegisterSink(DirectBufferSink<RenderTarget>::Make("renderTarget", m_RenderTarget));
		RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", m_DepthStencil));

		RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", m_RenderTarget));
		RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", m_DepthStencil));
	}

	// see the note on HorizontalBlurPass::Execute
	void VerticalBlurPass::Execute(Graphics & gfx) const noxnd
	{
		auto buf = m_Direction->GetBuffer();
		buf["isHorizontal"] = false;
		m_Direction->SetBuffer(buf);

		m_Direction->Bind(gfx);
		FullscreenPass::Execute(gfx);
	}
}