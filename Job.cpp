#include "Job.h"
#include "Step.h"
#include "Drawable.h"


namespace RenderGraph
{
	Job::Job(const Step* pStep, const Drawable* pDrawable)
		: m_pStep(pStep), m_pDrawable(pDrawable)
	{}

	void Job::Execute(Graphics& gfx) const noxnd
	{
		m_pDrawable->Bind(gfx);
		m_pStep->Bind(gfx);
		gfx.DrawIndexed(m_pDrawable->GetIndexCount());
	}
}