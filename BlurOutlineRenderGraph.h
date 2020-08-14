#pragma once
#include "RenderGraph.h"
#include <memory>
#include "ConstantBuffersEx.h"

class Graphics;
namespace Bind
{
	class Bindable;
	class RenderTarget;
}

namespace RenderGraph
{
	class BlurOutlineRenderGraph : public RenderGraph
	{
	public:
		BlurOutlineRenderGraph(Graphics& gfx);
	private:
		void SetKernelGauss(int radius, float sigma) noxnd;
		// private data
		static constexpr int s_MaxRadius = 7;
		static constexpr int s_Radius = 4;
		static constexpr float s_Sigma = 2.0f;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> m_BlurKernel;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> m_BlurDirection;
	};
}