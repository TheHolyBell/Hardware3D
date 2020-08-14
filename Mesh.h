#pragma once
#include "Drawable.h"
#include "Drawable.h"
#include "ConditionalNoexcept.h"

class Material;
struct aiMesh;

class Mesh : public Drawable
{
public:
	Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f) noxnd;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Submit(DirectX::FXMMATRIX accumulatedTransform) const noxnd;
private:
	mutable DirectX::XMFLOAT4X4 m_Transform;
};

//class Node
//{
//	friend class Model;
//public:
//	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noxnd;
//	void Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
//	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
//	const DirectX::XMFLOAT4X4& GetAppliedTransform() const noexcept;
//	int GetId() const noexcept;
//	void ShowTree(Node*& pSelectedNode) const noexcept;
//private:
//	void AddChild(std::unique_ptr<Node> pChild) noxnd;
//private:
//	std::string m_Name;
//	int m_ID;
//	std::vector<std::unique_ptr<Node>> m_ChildPtrs;
//	std::vector<Mesh*> m_MeshPtrs;
//	DirectX::XMFLOAT4X4 m_Transform;
//	DirectX::XMFLOAT4X4 m_AppliedTransform;
//};
//
//class Model
//{
//public:
//	Model(Graphics& gfx, const std::string& path, float scale = 1.0f);
//	void Submit(FrameCommander& frame) const noxnd;
//	void ShowWindow(Graphics& gfx, const char* windowName = nullptr) noexcept;
//	void SetRootTransform(DirectX::FXMMATRIX tf) noexcept;
//	~Model() noexcept;
//private:
//	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale);
//	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node) noexcept;
//private:
//	bool m_bShowGizmo = false;
//	DirectX::XMFLOAT4X4 m_Transform;
//	std::unique_ptr<Node> m_pRoot;
//	std::vector<std::unique_ptr<Mesh>> m_MeshPtrs;
//	std::unique_ptr<class ModelWindow> m_pWindow;
//};