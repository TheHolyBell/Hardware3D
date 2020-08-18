#pragma once
#include "Graphics.h"
#include "ConditionalNoexcept.h"
#include <memory>

class Model;
class Mesh;
class TechniqueProbe;
class ModelProbe;

class Node
{
	friend Model;
public:
	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noxnd;
	void Submit(size_t channels, DirectX::FXMMATRIX accumulatedTransform) const noxnd;
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;
	int GetId() const noexcept;
	bool HasChildren() const noexcept
	{
		return !m_ChildPtrs.empty();
	}
	void Accept(ModelProbe& probe);
	void Accept(TechniqueProbe& probe);
	const std::string& GetName() const
	{
		return m_Name;
	}
private:
	void AddChild(std::unique_ptr<Node> pChild) noxnd;
private:
	std::string m_Name;
	int m_ID;
	std::vector<std::unique_ptr<Node>> m_ChildPtrs;
	std::vector<Mesh*> m_MeshPtrs;
	DirectX::XMFLOAT4X4 m_Transform;
	DirectX::XMFLOAT4X4 m_AppliedTransform;
};