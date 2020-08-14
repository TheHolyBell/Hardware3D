#include "Step.h"
#include "Drawable.h"
#include "RenderGraph.h"
#include "TechniqueProbe.h"
#include "RenderQueuePass.h"

void Step::Submit(const Drawable& drawable) const
{
	m_pTargetPass->Accept(RenderGraph::Job{ this,&drawable });
}

void Step::InitializeParentReferences(const Drawable& parent) noexcept
{
	for (auto& b : m_Bindables)
	{
		b->InitializeParentReference(parent);
	}
}

Step::Step(const std::string& targetPassName)
	:
	m_TargetPassName{ targetPassName }
{}

Step::Step(const Step& src) noexcept
	:
	m_TargetPassName(src.m_TargetPassName)
{
	m_Bindables.reserve(src.m_Bindables.size());
	for (auto& pb : src.m_Bindables)
	{
		if (auto* pCloning = dynamic_cast<const Bind::CloningBindable*>(pb.get()))
		{
			m_Bindables.push_back(pCloning->Clone());
		}
		else
		{
			m_Bindables.push_back(pb);
		}
	}
}

void Step::AddBindable(std::shared_ptr<Bind::Bindable> bind_in) noexcept
{
	m_Bindables.push_back(std::move(bind_in));
}

void Step::Bind(Graphics& gfx) const noxnd
{
	for (const auto& b : m_Bindables)
	{
		b->Bind(gfx);
	}
}

void Step::Accept(TechniqueProbe& probe)
{
	probe.SetStep(this);
	for (auto& pb : m_Bindables)
	{
		pb->Accept(probe);
	}
}

void Step::Link(RenderGraph::RenderGraph& rg)
{
	assert(m_pTargetPass == nullptr);
	m_pTargetPass = &rg.GetRenderQueue(m_TargetPassName);
}