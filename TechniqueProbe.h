#pragma once
#include <limits>

namespace Dynamic
{
	class Buffer;
}

class TechniqueProbe
{
public:
	void SetTechnique(class Technique* pTech)
	{
		m_pTech = pTech;
		techIdx++;
		OnSetTechnique();
	}
	void SetStep(class Step* pStep)
	{
		m_pStep = pStep;
		stepIdx++;
		OnSetStep();
	}
	bool VisitBuffer(class Dynamic::Buffer& buffer)
	{
		bufIdx++;
		return OnVisitBuffer(buffer);
	}
protected:
	virtual void OnSetTechnique(){}
	virtual void OnSetStep(){}
	virtual bool OnVisitBuffer(class Dynamic::Buffer& buffer)
	{
		return false;
	}
protected:
	class Technique* m_pTech = nullptr;
	class Step* m_pStep = nullptr;
	size_t techIdx = std::numeric_limits<size_t>::max();
	size_t stepIdx = std::numeric_limits<size_t>::max();
	size_t bufIdx = std::numeric_limits<size_t>::max();
};