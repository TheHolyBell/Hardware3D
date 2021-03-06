#pragma once
#include "ConditionalNoexcept.h"

class Drawable;
class Graphics;
class Step;

namespace RenderGraph
{
	class Job
	{
	public:
		Job(const class Step* pStep, const class Drawable* pDrawable);
		void Execute(class Graphics& gfx) const noxnd;
	private:
		const class Drawable* m_pDrawable;
		const class Step* m_pStep;
	};
}