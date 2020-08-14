#pragma once
#include <limits>

namespace Dynamic
{
	class Buffer;
}

class Technique;
class Step;

class TechniqueProbe
{
public:
	// TODO: add callback for visiting each mesh

	void SetTechnique(Technique* pTech)
	{
		m_pTech = pTech;
		techIdx++;
		OnSetTechnique();
	}

	void SetStep(Step* pStep)
	{
		m_pStep = pStep;
		stepIdx++;
		OnSetStep();
	}

	bool VisitBuffer(Dynamic::Buffer& buffer)
	{
		bufIdx++;
		return OnVisitBuffer(buffer);
	}
protected:
	virtual void OnSetTechnique() {}
	virtual void OnSetStep() {}
	virtual bool OnVisitBuffer(Dynamic::Buffer& buffer)
	{
		return false;
	}
protected:
	Technique* m_pTech = nullptr;
	Step* m_pStep = nullptr;
	size_t techIdx = std::numeric_limits<size_t>::max();
	size_t stepIdx = std::numeric_limits<size_t>::max();
	size_t bufIdx = std::numeric_limits<size_t>::max();
};