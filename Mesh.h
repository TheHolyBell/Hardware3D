#pragma once
#include "ConditionalNoexcept.h"
#include "BindableCommon.h"
#include "Vertex.h"

#include <memory>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include "ChiliException.h"
#include <optional>

class ModelException : public ChiliException
{
public:
	ModelException(int line, const char* file, const std::string& note) noexcept;

	virtual const char* what() const noexcept override;
	virtual const char* GetType() const noexcept override;
	
	const std::string& GetNote() const noexcept;
private:
	std::string m_Note;
};

class Mesh : public Drawable
{
public:
	Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs);
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd;
	
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	mutable DirectX::XMFLOAT4X4 m_Transform;
};

class Node
{
	friend class Model;
public:
	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform) noxnd;
	void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd;
	void SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	int GetId() const noexcept;
	void ShowTree(Node*& pSelectedNode) const noexcept;
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

class Model
{
public:
	Model(Graphics& gfx, const std::string& filename);
	void Draw(Graphics& gfx) const noxnd;
	void ShowWindow(const char* windowName = nullptr) noexcept;
	void SetRootTransform(DirectX::FXMMATRIX transform) noexcept;
	~Model() noexcept;
private:
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node) noexcept;
private:
	std::unique_ptr<Node> m_pRoot;
	std::vector<std::unique_ptr<Mesh>> m_MeshPtrs;
	std::unique_ptr<class ModelWindow> m_pWindow;
};