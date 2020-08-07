#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include <memory>
#include "ConditionalNoexcept.h"
#include "Technique.h"

class TechniqueProbe;
class Material;
struct aiMesh;

namespace Bind
{
	class Bindable;
	class IndexBuffer;
	class Topology;
	class InputLayout;
	class VertexBuffer;
}

class Drawable
{
public:
	Drawable() = default;
	
	Drawable(const Drawable& rhs) = delete;
	Drawable& operator=(const Drawable& rhs) = delete;

	Drawable(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f) noexcept;

	void AddTechnique(Technique tech) noexcept;
	void Submit(class FrameCommander& frame) const noexcept;
	void Bind(Graphics& gfx) const noexcept;
	void Accept(TechniqueProbe& probe);
	UINT GetIndexCount() const noxnd;
	virtual ~Drawable();

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
protected:
	std::shared_ptr<Bind::IndexBuffer> m_pIndices;
	std::shared_ptr<Bind::VertexBuffer> m_pVertices;
	std::shared_ptr<Bind::Topology> m_pTopology;
	std::vector<Technique> m_Techniques;
};