#include "Mesh.h"
#include "ShaderReflector.h"
#include "ImGui\imgui.h"
#include "Texture.h"
#include <unordered_map>
#include <sstream>
#include <iostream>

namespace dx = DirectX;

ModelException::ModelException(int line, const char* file, const std::string& note) noexcept
	: ChiliException(line, file), m_Note(note)
{
}

const char* ModelException::what() const noexcept
{
	std::ostringstream oss;
	oss << ChiliException::what() << std::endl
		<< "[Note] " << GetNote();
	m_WhatBuffer = oss.str();
	return m_WhatBuffer.c_str();
}

const char* ModelException::GetType() const noexcept
{
	return "Chili Model Exception";
}

const std::string& ModelException::GetNote() const noexcept
{
	return m_Note;
}

Mesh::Mesh(Graphics& gfx, std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs)
{
	if (!IsStaticInitialized())
		AddStaticBind(std::make_unique<Bind::Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	for (auto& pb : bindPtrs)
	{
		if (auto pi = dynamic_cast<Bind::IndexBuffer*>(pb.get()))
		{
			AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer>{pi});
			pb.release();
		}
		else
			AddBind(std::move(pb));
	}

	AddBind(std::make_unique<Bind::TransformCbuf>(gfx, *this));
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&m_Transform);
}

void Mesh::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd
{
	DirectX::XMStoreFloat4x4(&m_Transform, accumulatedTransform);
	Drawable::Draw(gfx);
}

Node::Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform_in) noxnd
	:
	m_ID(id),
	m_MeshPtrs(std::move(meshPtrs)),
	m_Name(name)
{
	dx::XMStoreFloat4x4(&m_Transform, transform_in);
	dx::XMStoreFloat4x4(&m_AppliedTransform, dx::XMMatrixIdentity());
}

void Node::Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noxnd
{
	const auto finalTransform =
		dx::XMLoadFloat4x4(&m_AppliedTransform) *
		dx::XMLoadFloat4x4(&m_Transform) *
		accumulatedTransform;
	for (const auto pm : m_MeshPtrs)
	{
		pm->Draw(gfx, finalTransform);
	}
	for (const auto& pc : m_ChildPtrs)
	{
		pc->Draw(gfx, finalTransform);
	}
}

void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	dx::XMStoreFloat4x4(&m_AppliedTransform, transform);
}

int Node::GetId() const noexcept
{
	return m_ID;
}

void Node::ShowTree(Node*& pSelectedNode) const noexcept
{
	// if there is no selected node, set selectedId to an impossible value
	const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
	// build up flags for current node
	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ((GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((m_ChildPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);
	// render this node
	const auto expanded = ImGui::TreeNodeEx(
		(void*)(intptr_t)GetId(), node_flags, m_Name.c_str()
	);
	// processing for selecting node
	if (ImGui::IsItemClicked())
	{
		pSelectedNode = const_cast<Node*>(this);
	}
	// recursive rendering of open node's children
	if (expanded)
	{
		for (const auto& pChild : m_ChildPtrs)
		{
			pChild->ShowTree(pSelectedNode);
		}
		ImGui::TreePop();
	}
}

void Node::AddChild(std::unique_ptr<Node> pChild) noxnd
{
	assert(pChild != nullptr);
	m_ChildPtrs.push_back(std::move(pChild));
}


// Model
class ModelWindow // pImpl idiom, only defined in .cpp file
{
public:
	void Show(const char* windowName, const Node& root) noexcept
	{
		// window name defaults to "Model"
		windowName = windowName ? windowName : "Model";
		// need an ints to track node indices and selected node
		int nodeIndexTracker = 0;
		if (ImGui::Begin(windowName))
		{
			ImGui::Columns(2, nullptr, true);
			root.ShowTree(m_pSelectedNode);

			ImGui::NextColumn();
			if (m_pSelectedNode != nullptr)
			{
				auto& transform = m_Transforms[m_pSelectedNode->GetId()];
				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);
			}
		}
		ImGui::End();
	}
	dx::XMMATRIX GetTransform() const noexcept
	{
		const auto& transform = m_Transforms.at(*m_SelectedIndex);
		return
			dx::XMMatrixRotationRollPitchYaw(transform.pitch, transform.yaw, transform.roll) *
			dx::XMMatrixTranslation(transform.x, transform.y, transform.z);
	}
	Node* GetSelectedNode() const noexcept
	{
		return m_pSelectedNode;
	}
private:
	std::optional<int> m_SelectedIndex;
	Node* m_pSelectedNode;
	struct TransformParameters
	{
		float pitch = 0.0f;
		float yaw = 0.0f;
		float roll = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	std::unordered_map<int, TransformParameters> m_Transforms;
};

Model::Model(Graphics& gfx, const std::string& filename)
	: m_pWindow(std::make_unique<ModelWindow>())
{
	Assimp::Importer _Importer;
	const auto pScene = _Importer.ReadFile(filename.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals
	);

	if (pScene == nullptr)
		throw ModelException(__LINE__, __FILE__, _Importer.GetErrorString());

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		m_MeshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials));
	}


	int nextId = 0;
	m_pRoot = ParseNode(nextId, *pScene->mRootNode);
}

void Model::ShowWindow(const char* windowName) noexcept
{
	m_pWindow->Show(windowName, *m_pRoot);
}

Model::~Model()
{
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials)
{
	using Dynamic::VertexLayout;

	std::vector<std::unique_ptr<Bind::Bindable>> _BindablePtrs;
	std::vector<unsigned short> _Indices(mesh.mNumFaces * 3);

	auto pvs = std::make_unique<Bind::VertexShader>(gfx, L"PhongVS.cso");
	auto pvsbc = pvs->GetByteCode();
	_BindablePtrs.push_back(std::move(pvs));
	
	Dynamic::VertexBuffer vbuf{ ShaderReflector::GetLayoutFromShader(pvsbc) };

	for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
	{
		vbuf.EmplaceBack(
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
	}

	for (unsigned int i = 0; i < mesh.mNumFaces; ++i)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		_Indices.push_back(face.mIndices[0]);
		_Indices.push_back(face.mIndices[1]);
		_Indices.push_back(face.mIndices[2]);
	}

	_BindablePtrs.push_back(std::make_unique<Bind::VertexBuffer>(gfx, vbuf));
	_BindablePtrs.push_back(std::make_unique<Bind::IndexBuffer>(gfx, _Indices));
	_BindablePtrs.push_back(std::make_unique<Bind::InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pvsbc));

	bool _bHasSpecularMap = false;
	float shininess = 35.0f;
	if (mesh.mMaterialIndex >= 0)
	{
		auto& material = *pMaterials[mesh.mMaterialIndex];

		using namespace std::string_literals;
		const auto base = "Models\\nano_textured\\"s;
		aiString texFileName;
		
		material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName);
		_BindablePtrs.push_back(std::make_unique<Bind::Texture>(gfx, Surface::FromFile(base + texFileName.C_Str())));

		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
		{
			_BindablePtrs.push_back(std::make_unique<Bind::Texture>(gfx, Surface::FromFile(base + texFileName.C_Str()), 1));
			_bHasSpecularMap = true;
		}
		else
		{
			material.Get(AI_MATKEY_SHININESS, shininess);
		}

		_BindablePtrs.push_back(std::make_unique<Bind::Sampler>(gfx));
	}


	if (_bHasSpecularMap)
		_BindablePtrs.push_back(std::make_unique<Bind::PixelShader>(gfx, L"PhongPSSpecMap.cso"));
	else
	{
		_BindablePtrs.push_back(std::make_unique<Bind::PixelShader>(gfx, L"PhongPS.cso"));

		struct PSMaterialConstant
		{
			float specularIntensity = 0.8f;
			float specularPower;
			float padding[2];
		} pmc;
		pmc.specularPower = shininess;
		_BindablePtrs.push_back(std::make_unique<Bind::PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));
	}

	return std::make_unique<Mesh>(gfx, std::move(_BindablePtrs));
}

std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node) noexcept
{
	namespace dx = DirectX;
	const auto transform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	));

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
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
	}

	return pNode;
}

void Model::Draw(Graphics& gfx) const noxnd
{
	if (auto node = m_pWindow->GetSelectedNode())
		node->SetAppliedTransform(m_pWindow->GetTransform());
	m_pRoot->Draw(gfx, dx::XMMatrixIdentity());
}