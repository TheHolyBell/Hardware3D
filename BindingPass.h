#pragma once
#include "Pass.h"
#include "Sink.h"
#include <vector>
#include <memory>

namespace Bind
{
	class Bindable;
}

namespace RenderGraph
{
	class BindingPass : public Pass
	{
	protected:
		BindingPass(const std::string& name, std::vector<std::shared_ptr<Bind::Bindable>> binds = {});
		void AddBind(std::shared_ptr<Bind::Bindable> bind) noexcept;
		void BindAll(Graphics& gfx) const noxnd;
		virtual void Finalize() override;
	protected:
		template<typename T>
		void AddBindSink(const std::string& name)
		{
			const auto index = m_Binds.size();
			m_Binds.emplace_back();
			RegisterSink(std::make_unique<ContainerBindableSink<T>>(name, m_Binds, index));
		}
		std::shared_ptr<Bind::RenderTarget> m_RenderTarget;
		std::shared_ptr<Bind::DepthStencil> m_DepthStencil;
	private:
		void BindBufferResources(Graphics& gfx) const noxnd;
	private:
		std::vector<std::shared_ptr<Bind::Bindable>> m_Binds;
	};
}