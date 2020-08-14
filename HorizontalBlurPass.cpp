#include "HorizontalBlurPass.h"
#include "PixelShader.h"
#include "RenderTarget.h"
#include "Sink.h"
#include "Source.h"
#include "Blender.h"
#include "Sampler.h"
#include "ConstantBuffersEx.h"

using namespace Bind;

namespace RenderGraph
{
	HorizontalBlurPass::HorizontalBlurPass(const std::string& name, Graphics& gfx, unsigned int fullWidth, unsigned int fullHeight)
		: FullscreenPass(name, gfx)
	{
		AddBind(PixelShader::Resolve(gfx, "BlurOutline_PS.cso"));
		AddBind(Blender::Resolve(gfx, false));
		AddBind(Sampler::Resolve(gfx, Sampler::Type::Point, true));

		AddBindSink<Bind::RenderTarget>("scratchIn");
		AddBindSink<Bind::CachingPixelConstantBufferEx>("kernel");
		RegisterSink(DirectBindableSink<CachingPixelConstantBufferEx>::Make("direction", m_Direction));

		// the renderTarget is internally sourced and then exported as a Bindable
		m_RenderTarget = std::make_shared<Bind::ShaderInputRenderTarget>(gfx, fullWidth / 2, fullHeight / 2, 0u);
		RegisterSource(DirectBindableSource<RenderTarget>::Make("scratchOut", m_RenderTarget));
	}

	// this override is necessary because we cannot (yet) link input bindables directly into
	// the container of bindables (mainly because vector growth buggers references)
	void HorizontalBlurPass::Execute(Graphics & gfx) const noxnd
	{
		auto buf = m_Direction->GetBuffer();
		buf["isHorizontal"] = true;
		m_Direction->SetBuffer(buf);

		m_Direction->Bind(gfx);
		FullscreenPass::Execute(gfx);
	}
}