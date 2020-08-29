#pragma once
#include "RenderGraph.h"
#include <memory>
#include "ConstantBuffersEx.h"

class Graphics;
class Camera;
namespace Bind
{
	class Bindable;
	class RenderTarget;
	class ShadowSampler;
	class ShadowRasterizer;
}

namespace RenderGraph
{
	class BlurOutlineRenderGraph : public RenderGraph
	{
	public:
		BlurOutlineRenderGraph(Graphics& gfx);
		void RenderWindows(Graphics& gfx);
		void DumpShadowMap(Graphics& gfx, const std::string& path);
		void BindMainCamera(Camera& cam);
		void BindShadowCamera(Camera& cam);
	private:
		void RenderKernelWindow(Graphics& gfx);
		// private functions
		void SetKernelGauss(int radius, float sigma) noxnd;
		void SetKernelBox(int radius) noxnd;
		// private data
		enum class KernelType
		{
			Gauss,
			Box,
		} m_KernelType = KernelType::Gauss;
		static constexpr int s_MaxRadius = 7;
		int m_Radius = 4;
		float m_Sigma = 2.0f;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> m_BlurKernel;
		std::shared_ptr<Bind::CachingPixelConstantBufferEx> m_BlurDirection;
	};
}