#include "Model.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include "ModelException.h"
#include "Node.h"
#include "Mesh.h"
#include "Material.h"
#include "ChiliXM.h"

namespace dx = DirectX;

Model::Model(Graphics& gfx, const std::string& pathString, const float scale)
{
	m_Name = std::filesystem::path(pathString).filename().string();


	Assimp::Importer _Importer;
	const auto pScene = _Importer.ReadFile(pathString,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	if (pScene == nullptr)
		throw ModelException(__LINE__, __FILE__, _Importer.GetErrorString());

	// Parse materials
	std::vector<Material> _Materials;
	_Materials.reserve(pScene->mNumMaterials);
	for (size_t i = 0; i < pScene->mNumMaterials; ++i)
	{
		_Materials.emplace_back(gfx, *pScene->mMaterials[i], pathString);
	}

	for (size_t i = 0; i < pScene->mNumMeshes; ++i)
	{
		const auto& mesh = *pScene->mMeshes[i];
		m_MeshPtrs.push_back(std::make_unique<Mesh>(gfx, _Materials[mesh.mMaterialIndex], mesh, scale));
	}

	int nextId = 0;
	m_pRoot = ParseNode(nextId, *pScene->mRootNode, scale);
}

void Model::Submit(size_t channels) const noxnd
{
	m_pRoot->Submit(channels,dx::XMMatrixIdentity());
}

void Model::SetRootTransform(DirectX::FXMMATRIX tf) noexcept
{
	m_pRoot->SetAppliedTransform(tf);
}

void Model::Accept(ModelProbe& probe)
{
	m_pRoot->Accept(probe);
}

void Model::LinkTechniques(RenderGraph::RenderGraph& renderGraph)
{
	for (auto& pMesh : m_MeshPtrs)
		pMesh->LinkTechniques(renderGraph);
}

std::string Model::GetName() const
{
	return m_Name;
}

Model::~Model() noexcept
{}

std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node, float scale) noexcept
{
	namespace dx = DirectX;
	const auto transform = ScaleTranslation(dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	)), scale);

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(m_MeshPtrs.at(meshIdx).get());
	}

	auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i], scale));
	}

	return pNode;
}