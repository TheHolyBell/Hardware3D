#pragma once
#include "RenderQueuePass.h"
#include "Job.h"
#include <vector>
#include "PixelShader.h"
#include "VertexShader.h"
#include "Stencil.h"
#include "Rasterizer.h"
#include "Source.h"
#include "RenderTarget.h"
#include "Surface.h"
#include "Blender.h"
#include "NullPixelShader.h"
#include "Camera.h"
#include <iostream>

class Graphics;

namespace RenderGraph
{
	class ShadowMappingPass : public RenderQueuePass
	{
	public:
		void BindShadowCamera(const Camera& cam) noexcept
		{
			m_pShadowCamera = &cam;
		}
		ShadowMappingPass(Graphics& gfx, const std::string& name)
			:
			RenderQueuePass(std::move(name))
		{
			using namespace Bind;
			m_DepthStencil = std::make_unique<ShaderInputDepthStencil>(gfx, 3, DepthStencil::Usage::ShadowDepth);
			AddBind(VertexShader::Resolve(gfx, "Solid_VS.cso"));
			AddBind(NullPixelShader::Resolve(gfx));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
			AddBind(Blender::Resolve(gfx, false));
			RegisterSource(DirectBindableSource<Bind::DepthStencil>::Make("map", m_DepthStencil));
		}
		void Execute(Graphics& gfx) const noxnd override
		{
			m_DepthStencil->Clear(gfx);
			m_pShadowCamera->BindToGraphics(gfx);
			RenderQueuePass::Execute(gfx);
		}
		void DumpShadowMap(Graphics& gfx, const std::string& path) const
		{
			m_DepthStencil->ToSurface(gfx).Save(path);
		}
		ID3D11ShaderResourceView* GetUnderlyingHandle() const
		{
			return dynamic_cast<Bind::ShaderInputDepthStencil*>(m_DepthStencil.get())->GetHandle();
		}
	private:
		const Camera* m_pShadowCamera = nullptr;
	};
}