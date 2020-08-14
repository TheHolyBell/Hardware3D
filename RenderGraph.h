#pragma once
#include <string>
#include <vector>
#include <memory>
#include "ConditionalNoexcept.h"

class Graphics;

namespace Bind
{
	class RenderTarget;
	class DepthStencil;
}

namespace RenderGraph
{
	class Pass;
	class RenderQueuePass;
	class Source;
	class Sink;

	class RenderGraph
	{
	public:
		RenderGraph(Graphics& gfx);
		~RenderGraph();

		void Execute(Graphics& gfx) noxnd;
		void Reset() noexcept;
		RenderQueuePass& GetRenderQueue(const std::string& passName);
	protected:
		void SetSinkTarget(const std::string& sinkName, const std::string& target);
		void AddGlobalSource(std::unique_ptr<Source> out);
		void AddGlobalSink(std::unique_ptr<Sink> in);
		void Finalize();
		void AppendPass(std::unique_ptr<Pass> pass);
	private:
		void LinkSinks(Pass& pass);
		void LinkGlobalSinks();
	private:
		std::vector<std::unique_ptr<Pass>> m_Passes;
		std::vector<std::unique_ptr<Source>> m_GlobalSources;
		std::vector<std::unique_ptr<Sink>> m_GlobalSinks;
		std::shared_ptr<Bind::RenderTarget> m_pBackBufferTarget;
		std::shared_ptr<Bind::DepthStencil> m_pMasterDepth;
		bool m_bFinalized = false;
	};
}