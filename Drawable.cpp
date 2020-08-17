#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "BindableCommon.h"
#include "BindableCodex.h"
#include "Material.h"
#include <assimp\scene.h>

using namespace Bind;

void Drawable::Submit(size_t channelFilter) const noexcept
{
	for (const auto& tech : m_Techniques)
	{
		tech.Submit(*this, channelFilter);
	}
}

Drawable::Drawable(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale) noexcept
{
	m_pVertices = mat.MakeVertexBindable(gfx, mesh, scale);
	m_pIndices = mat.MakeIndexBindable(gfx, mesh);
	m_pTopology = Bind::Topology::Resolve(gfx);

	for (auto& t : mat.GetTechniques())
	{
		AddTechnique(std::move(t));
	}
}

void Drawable::AddTechnique(Technique tech_in) noexcept
{
	tech_in.InitializeParentReferences(*this);
	m_Techniques.push_back(std::move(tech_in));
}

void Drawable::Bind(Graphics& gfx) const noxnd
{
	m_pTopology->Bind(gfx);
	m_pIndices->Bind(gfx);
	m_pVertices->Bind(gfx);
}

void Drawable::Accept(TechniqueProbe& probe)
{
	for (auto& t : m_Techniques)
	{
		t.Accept(probe);
	}
}

UINT Drawable::GetIndexCount() const noxnd
{
	return m_pIndices->GetCount();
}

void Drawable::LinkTechniques(RenderGraph::RenderGraph& rg)
{
	for (auto& tech : m_Techniques)
	{
		tech.Link(rg);
	}
}

Drawable::~Drawable()
{}