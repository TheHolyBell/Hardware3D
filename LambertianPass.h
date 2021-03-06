#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include <vector>
#include "Sink.h"
#include "Source.h"
#include "Stencil.h"
#include "ShadowCameraCBuf.h"
#include "ShadowSampler.h"
#include "Camera.h"

class Graphics;

namespace RenderGraph
{
	class LambertianPass : public RenderQueuePass
	{
	public:
		LambertianPass(Graphics& gfx, const std::string& name)
			:
			RenderQueuePass(name),
			m_pShadowCBuf(std::make_shared<Bind::ShadowCameraCBuf>(gfx))
		{
			using namespace Bind;
			AddBind(m_pShadowCBuf);
			RegisterSink(DirectBufferSink<RenderTarget>::Make("renderTarget", m_RenderTarget));
			RegisterSink(DirectBufferSink<DepthStencil>::Make("depthStencil", m_DepthStencil));
			AddBindSink<Bind::Bindable>("shadowMap");
			AddBind(std::make_shared<Bind::ShadowSampler>(gfx));
			RegisterSource(DirectBufferSource<RenderTarget>::Make("renderTarget", m_RenderTarget));
			RegisterSource(DirectBufferSource<DepthStencil>::Make("depthStencil", m_DepthStencil));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
		}
		void BindMainCamera(const Camera& cam) noexcept
		{
			m_pMainCamera = &cam;
		}
		void BindShadowCamera(const Camera& cam) noexcept
		{
			m_pShadowCBuf->SetCamera(&cam);
		}
		void Execute(Graphics& gfx) const noxnd override
		{
			assert(m_pMainCamera);
			m_pShadowCBuf->Update(gfx);
			m_pMainCamera->BindToGraphics(gfx);
			RenderQueuePass::Execute(gfx);
		}
	private:
		std::shared_ptr<Bind::ShadowCameraCBuf> m_pShadowCBuf;
		const Camera* m_pMainCamera = nullptr;
	};
}