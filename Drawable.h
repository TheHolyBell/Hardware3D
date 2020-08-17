#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include <memory>
#include "ConditionalNoexcept.h"
#include "Technique.h"

class TechniqueProbe;
class Material;
struct aiMesh;

namespace RenderGraph
{
	class RenderGraph;
}

namespace Bind
{
	class IndexBuffer;
	class VertexBuffer;
	class Topology;
	class InputLayout;
}

class Drawable
{
public:
	Drawable() = default;

	Drawable(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f) noexcept;
	
	Drawable(const Drawable& rhs) = delete;
	Drawable& operator=(const Drawable& rhs) = delete;

	Drawable(Drawable&& rhs) = delete;
	Drawable& operator=(Drawable&& rhs) = delete;

	void AddTechnique(Technique tech) noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Submit(size_t channelFilter) const noexcept;
	void Bind(Graphics& gfx) const noxnd;
	void Accept(TechniqueProbe& probe);
	UINT GetIndexCount() const noxnd;
	void LinkTechniques(RenderGraph::RenderGraph& renderGraph);
	virtual ~Drawable();
protected:
	std::shared_ptr<Bind::IndexBuffer> m_pIndices;
	std::shared_ptr<Bind::VertexBuffer> m_pVertices;
	std::shared_ptr<Bind::Topology> m_pTopology;
	std::vector<Technique> m_Techniques;
};