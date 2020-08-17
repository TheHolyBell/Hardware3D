#include "BlurOutlineRenderGraph.h"
#include "BufferClearPass.h"
#include "LambertianPass.h"
#include "OutlineDrawingPass.h"
#include "OutlineMaskGenerationPass.h"
#include "Source.h"
#include "HorizontalBlurPass.h"
#include "VerticalBlurPass.h"
#include "BlurOutlineDrawingPass.h"
#include "WireframePass.h"
#include "RenderTarget.h"
#include "DynamicConstant.h"
#include "ChiliMath.h"
#include "ImGui/imgui.h"

namespace RenderGraph
{
	BlurOutlineRenderGraph::BlurOutlineRenderGraph(Graphics& gfx)
		:
		RenderGraph(gfx)
	{
		{
			auto pass = std::make_unique<BufferClearPass>("clearRT");
			pass->SetSinkLinkage("buffer", "$.backbuffer");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<BufferClearPass>("clearDS");
			pass->SetSinkLinkage("buffer", "$.masterDepth");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<LambertianPass>(gfx, "lambertian");
			pass->SetSinkLinkage("renderTarget", "clearRT.buffer");
			pass->SetSinkLinkage("depthStencil", "clearDS.buffer");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<OutlineMaskGenerationPass>(gfx, "outlineMask");
			pass->SetSinkLinkage("depthStencil", "lambertian.depthStencil");
			AppendPass(std::move(pass));
		}

		// setup blur constant buffers
		{
			{
				Dynamic::RawLayout l;
				l.Add<Dynamic::Integer>("nTaps");
				l.Add<Dynamic::Array>("coefficients");
				l["coefficients"].Set<Dynamic::Float>(s_MaxRadius * 2 + 1);
				Dynamic::Buffer buf{ std::move(l) };
				m_BlurKernel = std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 0);
				SetKernelGauss(m_Radius, m_Sigma);
				AddGlobalSource(DirectBindableSource<Bind::CachingPixelConstantBufferEx>::Make("blurKernel", m_BlurKernel));
			}
			{
				Dynamic::RawLayout l;
				l.Add<Dynamic::Bool>("isHorizontal");
				Dynamic::Buffer buf{ std::move(l) };
				m_BlurDirection = std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 1);
				AddGlobalSource(DirectBindableSource<Bind::CachingPixelConstantBufferEx>::Make("blurDirection", m_BlurDirection));
			}
		}

		{
			auto pass = std::make_unique<BlurOutlineDrawingPass>(gfx, "outlineDraw", gfx.GetWidth(), gfx.GetHeight());
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<HorizontalBlurPass>("horizontal", gfx, gfx.GetWidth(), gfx.GetHeight());
			pass->SetSinkLinkage("scratchIn", "outlineDraw.scratchOut");
			pass->SetSinkLinkage("kernel", "$.blurKernel");
			pass->SetSinkLinkage("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<VerticalBlurPass>("vertical", gfx);
			pass->SetSinkLinkage("renderTarget", "lambertian.renderTarget");
			pass->SetSinkLinkage("depthStencil", "outlineMask.depthStencil");
			pass->SetSinkLinkage("scratchIn", "horizontal.scratchOut");
			pass->SetSinkLinkage("kernel", "$.blurKernel");
			pass->SetSinkLinkage("direction", "$.blurDirection");
			AppendPass(std::move(pass));
		}
		{
			auto pass = std::make_unique<WireframePass>(gfx, "wireframe");
			pass->SetSinkLinkage("renderTarget", "vertical.renderTarget");
			pass->SetSinkLinkage("depthStencil", "vertical.depthStencil");
			AppendPass(std::move(pass));
		}
		SetSinkTarget("backbuffer", "wireframe.renderTarget");

		Finalize();
	}

	void BlurOutlineRenderGraph::RenderWidgets(Graphics& gfx)
	{
		if (ImGui::Begin("Kernel"))
		{
			bool _bFilterChanged = false;
			{
				const char* items[] = { "Gauss","Box" };
				static const char* _CurItem = items[0];
				if (ImGui::BeginCombo("Filter Type", _CurItem))
				{
					for (int n = 0; n < std::size(items); n++)
					{
						const bool isSelected = (_CurItem == items[n]);
						if (ImGui::Selectable(items[n], isSelected))
						{
							_bFilterChanged = true;
							_CurItem = items[n];
							if (_CurItem == items[0])
							{
								m_KernelType = KernelType::Gauss;
							}
							else if (_CurItem == items[1])
							{
								m_KernelType = KernelType::Box;
							}
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}

			bool _bRadChange = ImGui::SliderInt("Radius", &m_Radius, 0, s_MaxRadius);
			bool _bSigChange = ImGui::SliderFloat("Sigma", &m_Sigma, 0.1f, 10.0f);
			if (_bRadChange || _bSigChange || _bFilterChanged)
			{
				if (m_KernelType == KernelType::Gauss)
				{
					SetKernelGauss(m_Radius, m_Sigma);
				}
				else if (m_KernelType == KernelType::Box)
				{
					SetKernelBox(m_Radius);
				}
			}
		}
		ImGui::End();
	}

	void BlurOutlineRenderGraph::SetKernelGauss(int radius, float sigma) noxnd
	{
		assert(radius <= s_MaxRadius);
		auto k = m_BlurKernel->GetBuffer();
		const int nTaps = radius * 2 + 1;
		k["nTaps"] = nTaps;
		float sum = 0.0f;
		for (int i = 0; i < nTaps; i++)
		{
			const auto x = float(i - radius);
			const auto g = gauss(x, sigma);
			sum += g;
			k["coefficients"][i] = g;
		}
		for (int i = 0; i < nTaps; i++)
		{
			k["coefficients"][i] = (float)k["coefficients"][i] / sum;
		}
		m_BlurKernel->SetBuffer(k);
	}

	void BlurOutlineRenderGraph::SetKernelBox(int radius) noxnd
	{
		assert(m_Radius <= s_MaxRadius);
		auto k = m_BlurKernel->GetBuffer();
		const int nTaps = m_Radius * 2 + 1;
		k["nTaps"] = nTaps;
		const float c = 1.0f / nTaps;
		for (int i = 0; i < nTaps; ++i)
			k["coefficients"][i] = c;
		m_BlurKernel->SetBuffer(k);
	}
}