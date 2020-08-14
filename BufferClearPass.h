#pragma once
#include "Pass.h"
#include <memory>

namespace Bind
{
	class BufferResource;
}

namespace RenderGraph
{
	class BufferClearPass : public Pass
	{
	public:
		BufferClearPass(const std::string& name);
		virtual void Execute(Graphics& gfx) const noxnd override;
	private:
		std::shared_ptr<Bind::BufferResource> m_Buffer;
	};
}