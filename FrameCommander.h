#pragma once
#include <array>
#include "BindableCommon.h"
#include "Graphics.h"
#include "Job.h"
#include "Pass.h"
#include "PerfLog.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "Sampler.h"
#include "Blender.h"
#include "BlurPack.h"
#include "Job.h"
#include "EventDispatcher.h"

class FrameCommander
{
public:
	FrameCommander(Graphics& gfx)
		: 
		m_DepthStencil(gfx, gfx.GetWidth(), gfx.GetHeight()),
		m_RenderTarget1({gfx, gfx.GetWidth() / m_DownFactor, gfx.GetHeight() / m_DownFactor}),
		m_RenderTarget2({gfx, gfx.GetWidth() / m_DownFactor, gfx.GetHeight() / m_DownFactor}),
		m_BlurPack(gfx, 7, 2.6f, "BlurOutline_PS.cso")
	{
		namespace dx = DirectX;

		EventDispatcher::RegisterOnResize([&](int Width, int Height) 
		{ 
			static Graphics& _Gfx = gfx; 
			m_DepthStencil.Resize(gfx, Width, Height); 
			m_RenderTarget1->Resize(gfx, gfx.GetWidth() / m_DownFactor, gfx.GetHeight() / m_DownFactor );
			m_RenderTarget2->Resize(gfx, gfx.GetWidth() / m_DownFactor, gfx.GetHeight() / m_DownFactor);
		});

		// Setup fullscreen geometry
		Dynamic::VertexLayout _Layout;
		_Layout.Append(Dynamic::VertexLayout::Position2D);
		Dynamic::VertexBuffer bufFull{ _Layout };
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1, -1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1, 1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{  1, 1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{  1, -1 });
		m_pVbFull = Bind::VertexBuffer::Resolve(gfx, "$Full", std::move(bufFull));
		std::vector<unsigned short> _Indices = { 0, 1, 2, 0, 2, 3 };
		m_pIbFull = Bind::IndexBuffer::Resolve(gfx, "$Full", std::move(_Indices));
		
		// Setup fullscreen shaders
		m_pVsFull = Bind::VertexShader::Resolve(gfx, "Fullscreen_VS.cso");
		m_pLayoutFull = Bind::InputLayout::Resolve(gfx, _Layout, m_pVsFull->GetByteCode());
		m_pSamplerFullPoint = Bind::Sampler::Resolve(gfx, false, true);
		m_pSamplerFullBilin = Bind::Sampler::Resolve(gfx, true, true);
		m_pBlenderMerge = Bind::Blender::Resolve(gfx, true);
	}
	void Accept(Job job, size_t target) noexcept
	{
		m_Passes[target].Accept(job);
	}
	void Execute(Graphics& gfx) noxnd
	{
		using namespace Bind;

		// Setup render target used for normal passes
		m_DepthStencil.Clear(gfx);
		m_RenderTarget1->Clear(gfx);
		gfx.BindSwapBuffer(m_DepthStencil);
		// Main Phong lighting pass
		Blender::Resolve(gfx, false)->Bind(gfx);
		Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
		m_Passes[0].Execute(gfx);

		// Outline masking pass
		Stencil::Resolve(gfx, Stencil::Mode::Write)->Bind(gfx);
		NullPixelShader::Resolve(gfx)->Bind(gfx);
		m_Passes[1].Execute(gfx);

		// Outline drawing pass
		m_RenderTarget1->BindAsTarget(gfx);
		Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
		m_Passes[2].Execute(gfx);
		// Fullscreen blur horizontal pass
		m_RenderTarget2->BindAsTarget(gfx);
		m_RenderTarget1->BindAsTexture(gfx, 0);

		m_pVbFull->Bind(gfx);
		m_pIbFull->Bind(gfx);
		m_pVsFull->Bind(gfx);
		m_pLayoutFull->Bind(gfx);
		m_pSamplerFullPoint->Bind(gfx);
		m_BlurPack.Bind(gfx);
		m_BlurPack.SetHorizontal(gfx);
		gfx.DrawIndexed(m_pIbFull->GetCount());

		// Fullscreen blur vertical pass + combine
		gfx.BindSwapBuffer(m_DepthStencil);
		m_RenderTarget2->BindAsTexture(gfx, 0u);
		m_pBlenderMerge->Bind(gfx);
		m_pSamplerFullBilin->Bind(gfx);
		Stencil::Resolve(gfx, Stencil::Mode::Mask)->Bind(gfx);
		m_BlurPack.SetVertical(gfx);
		gfx.DrawIndexed(m_pIbFull->GetCount());
	}
	void Reset() noexcept
	{
		for (auto& p : m_Passes)
			p.Reset();
	}

	void ShowWindows(Graphics& gfx)
	{
		if (ImGui::Begin("Blur"))
		{
			if (ImGui::SliderInt("Down Factor", &m_DownFactor, 1, 16))
			{
				m_RenderTarget1->Resize(gfx, gfx.GetWidth() / m_DownFactor, gfx.GetHeight() / m_DownFactor);
				m_RenderTarget2->Resize(gfx, gfx.GetWidth() / m_DownFactor, gfx.GetHeight() / m_DownFactor);
			}
			m_BlurPack.RenderWidgets(gfx);
		}
		ImGui::End();
	}
private:
	std::array<Pass, 3> m_Passes;
	int m_DownFactor = 1;
	DepthStencil m_DepthStencil;
	std::optional<RenderTarget> m_RenderTarget1;
	std::optional<RenderTarget> m_RenderTarget2;
	BlurPack m_BlurPack;

	std::shared_ptr<Bind::VertexBuffer> m_pVbFull;
	std::shared_ptr<Bind::IndexBuffer> m_pIbFull;
	std::shared_ptr<Bind::VertexShader> m_pVsFull;
	std::shared_ptr<Bind::InputLayout> m_pLayoutFull;
	std::shared_ptr<Bind::Sampler> m_pSamplerFullPoint;
	std::shared_ptr<Bind::Sampler> m_pSamplerFullBilin;
	std::shared_ptr<Bind::Blender> m_pBlenderMerge;
};