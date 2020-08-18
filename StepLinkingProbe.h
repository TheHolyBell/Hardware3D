#pragma once
#include "TechniqueProbe.h"

namespace RenderGraph
{
	class RenderGraph;
}

class StepLinkingProbe : public TechniqueProbe
{
protected:
	virtual void OnSetStep() override;
private:
	RenderGraph::RenderGraph& m_RenderGraph;
};