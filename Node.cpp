#include "Node.h"
#include "Mesh.h"
#include "ModelProbe.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

Node::Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform_in) noxnd
	:
	m_ID(id),
	m_MeshPtrs(std::move(meshPtrs)),
	m_Name(name)
{
	dx::XMStoreFloat4x4(&m_Transform, transform_in);
	dx::XMStoreFloat4x4(&m_AppliedTransform, dx::XMMatrixIdentity());
}

void Node::Submit(size_t channels, DirectX::FXMMATRIX accumulatedTransform) const noxnd
{
	const auto built =
		dx::XMLoadFloat4x4(&m_AppliedTransform) *
		dx::XMLoadFloat4x4(&m_Transform) *
		accumulatedTransform;
	for (const auto pm : m_MeshPtrs)
	{
		pm->Submit(channels,built);
	}
	for (const auto& pc : m_ChildPtrs)
	{
		pc->Submit(channels, built);
	}
}

void Node::AddChild(std::unique_ptr<Node> pChild) noxnd
{
	assert(pChild);
	m_ChildPtrs.push_back(std::move(pChild));
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	dx::XMStoreFloat4x4(&m_AppliedTransform, transform);
}

const DirectX::XMFLOAT4X4& Node::GetAppliedTransform() const noexcept
{
	return m_AppliedTransform;
}

int Node::GetId() const noexcept
{
	return m_ID;
}

void Node::Accept(ModelProbe& probe)
{
	if (probe.PushNode(*this))
	{
		for (auto& cp : m_ChildPtrs)
		{
			cp->Accept(probe);
		}
		probe.PopNode(*this);
	}
}

void Node::Accept(TechniqueProbe& probe)
{
	for (auto& mp : m_MeshPtrs)
	{
		mp->Accept(probe);
	}
}