#include "Mesh.h"
#include "ShaderReflector.h"
#include "ImGui\imgui.h"
#include "ImGui\ImGuizmo.h"
#include "Texture.h"
#include <unordered_map>
#include <sstream>
#include <iostream>
#include "Graphics.h"
#include "ChiliXM.h"
#include "BindableCommon.h"

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

Mesh::Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs)
{
	AddBind(Bind::Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	for (auto& pb : bindPtrs)
		AddBind(std::move(pb));

	AddBind(std::make_shared<Bind::TransformCbuf>(gfx, *this));
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

const DirectX::XMFLOAT4X4& Node::GetAppliedTransform() const noexcept
{
	return m_AppliedTransform;
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
	void Show(Graphics& gfx, const char* windowName, const Node& root) noexcept
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
				const auto id = m_pSelectedNode->GetId();
				auto i = m_Transforms.find(id);
				if (i == m_Transforms.end())
				{
					const auto& applied = m_pSelectedNode->GetAppliedTransform();
					const auto angles = ExtractEulerAngles(applied);
					const auto translation = ExtractTranslation(applied);
					TransformParameters tp;
					tp.roll = angles.z;
					tp.pitch = angles.x;
					tp.yaw = angles.y;
					tp.x = translation.x;
					tp.y = translation.y;
					tp.z = translation.z;
					std::tie(i, std::ignore) = m_Transforms.insert({ id,tp });
				}
				auto& transform = m_Transforms[m_pSelectedNode->GetId()];
				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f);
				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f);

				if (!m_pSelectedNode->ControlMeDaddy(gfx, m_SkinMaterial))
				{
					m_pSelectedNode->ControlMeDaddy(gfx, m_RingMaterial);
				}
			}
		}
		ImGui::End();
	}
	dx::XMMATRIX GetTransform() const noexcept
	{
		assert(m_pSelectedNode != nullptr);
		const auto& transform = m_Transforms.at(*m_SelectedIndex);
		return
			dx::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
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
		float roll = 0.0f;
		float pitch = 0.0f;
		float yaw = 0.0f;
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
	};
	Node::PSMaterialConstantFullmonte m_SkinMaterial;
	Node::PSMaterialConstantNotex m_RingMaterial;
	std::unordered_map<int, TransformParameters> m_Transforms;
};

Model::Model(Graphics& gfx, const std::string& path, float scale)
	: m_pWindow(std::make_unique<ModelWindow>())
{
	Assimp::Importer _Importer;
	const auto pScene = _Importer.ReadFile(path,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	if (pScene == nullptr)
		throw ModelException(__LINE__, __FILE__, _Importer.GetErrorString());

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		m_MeshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials, path, scale));
	}


	int nextId = 0;
	m_pRoot = ParseNode(nextId, *pScene->mRootNode);
	DirectX::XMStoreFloat4x4(&m_Transform, DirectX::XMMatrixIdentity());
}

void Model::ShowWindow(Graphics& gfx, const char* windowName) noexcept
{
	m_pWindow->Show(gfx, windowName, *m_pRoot);
	Model* _this = const_cast<Model*>(this);

	if (ImGui::Begin((std::string("Miscelanious: #") + windowName).c_str()))
	{
		ImGui::Checkbox("Show Gizmo", &_this->m_bShowGizmo);
		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
		if (ImGui::IsKeyPressed('T'))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed('R'))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed('L')) // r Key
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
		ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<const float*>(m_Transform.m), matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("Translation", matrixTranslation, 3);
		ImGui::InputFloat3("Rotation", matrixRotation, 3);
		ImGui::InputFloat3("Scaling", matrixScale, 3);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, reinterpret_cast<float*>(_this->m_Transform.m));

		if (mCurrentGizmoOperation != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
				mCurrentGizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
				mCurrentGizmoMode = ImGuizmo::WORLD;
		}

		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

		if (m_bShowGizmo)
		{
			DirectX::XMFLOAT4X4 _ViewMatrix;
			DirectX::XMFLOAT4X4 _ProjectionMatrix;

			DirectX::XMStoreFloat4x4(&_ViewMatrix, gfx.GetCamera());
			DirectX::XMStoreFloat4x4(&_ProjectionMatrix, gfx.GetProjection());

			ImGuizmo::Manipulate(reinterpret_cast<const float*>(_ViewMatrix.m), reinterpret_cast<const float*>(_ProjectionMatrix.m), mCurrentGizmoOperation, mCurrentGizmoMode, reinterpret_cast<float*>(_this->m_Transform.m), nullptr, nullptr);

		}
	}
	ImGui::End();
}

void Model::SetRootTransform(DirectX::FXMMATRIX transform) noexcept
{
	m_pRoot->SetAppliedTransform(transform);
}

Model::~Model() noexcept
{
}

std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale)
{
	using namespace std::string_literals;
	using Dynamic::VertexLayout;
	using namespace Bind;

	std::vector<std::shared_ptr<Bindable>> _BindablePtrs;

	const auto rootPath = path.parent_path().string() + "\\";

	bool _bHasSpecularMap = false;
	bool _bHasAlphaGloss = false;
	bool _bHasAlphaDiffuse = false;
	bool _bHasNormalMap = false;
	bool _bHasDiffuseMap = false;
	float shininess = 2.0f;
	dx::XMFLOAT4 specularColor = { 0.18f,0.18f,0.18f,1.0f };
	dx::XMFLOAT4 diffuseColor = { 0.45f,0.45f,0.85f,1.0f };
	if (mesh.mMaterialIndex >= 0)
	{
		auto& material = *pMaterials[mesh.mMaterialIndex];

		aiString texFileName;

		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
		{
			auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str());
			_bHasAlphaDiffuse = tex->HasAlpha();
			_BindablePtrs.push_back(std::move(tex));
			_bHasDiffuseMap = true;
		}
		else
		{
			material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(diffuseColor));
		}

		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
		{
			auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 1);
			_bHasAlphaGloss = tex->HasAlpha();
			_BindablePtrs.push_back(std::move(tex));
			_bHasSpecularMap = true;
		}
		else
		{
			material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(specularColor));
		}
		if (!_bHasAlphaGloss)
		{
			material.Get(AI_MATKEY_SHININESS, shininess);
		}

		if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
		{
			auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 2);
			_bHasAlphaGloss = tex->HasAlpha();
			_BindablePtrs.push_back(std::move(tex));
			_bHasNormalMap = true;
		}

		if (_bHasDiffuseMap || _bHasSpecularMap || _bHasNormalMap)
		{
			_BindablePtrs.push_back(Bind::Sampler::Resolve(gfx));
		}
	}

	const auto meshTag = path.string() + "%" + mesh.mName.C_Str();

	if (_bHasDiffuseMap && _bHasNormalMap && _bHasSpecularMap)
	{
		auto pvs = VertexShader::Resolve(gfx, "PhongVSNormalMap.cso");
		auto pvsbc = pvs->GetByteCode();
		_BindablePtrs.push_back(std::move(pvs));
		Dynamic::VertexBuffer vbuf(ShaderReflector::GetLayoutFromShader(pvsbc));

		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				dx::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mBitangents[i]),
				*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		_BindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));

		_BindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));


		_BindablePtrs.push_back(PixelShader::Resolve(gfx,
			_bHasAlphaDiffuse ? "PhongPSSpecNormMask.cso" : "PhongPSSpecNormalMap.cso"
		));

		_BindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

		Node::PSMaterialConstantFullmonte pmc;
		pmc.specularPower = shininess;
		pmc.hasGlossMap = _bHasAlphaGloss ? TRUE : FALSE;
		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		_BindablePtrs.push_back(PixelConstantBuffer<Node::PSMaterialConstantFullmonte>::Resolve(gfx, pmc, 1u));
	}
	else if (_bHasDiffuseMap && _bHasNormalMap)
	{
		auto pvs = VertexShader::Resolve(gfx, "PhongVSNormalMap.cso");
		auto pvsbc = pvs->GetByteCode();
		_BindablePtrs.push_back(std::move(pvs));
		Dynamic::VertexBuffer vbuf(ShaderReflector::GetLayoutFromShader(pvsbc));

		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				dx::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mBitangents[i]),
				*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		_BindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));

		_BindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));


		_BindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSNormalMap.cso"));

		_BindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

		struct PSMaterialConstantDiffnorm
		{
			float specularIntensity;
			float specularPower;
			BOOL  normalMapEnabled = TRUE;
			float padding[1];
		} pmc;
		pmc.specularPower = shininess;
		pmc.specularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		_BindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstantDiffnorm>::Resolve(gfx, pmc, 1u));
	}
	else if (_bHasDiffuseMap && !_bHasNormalMap && _bHasSpecularMap)
	{
		auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
		auto pvsbc = pvs->GetByteCode();
		_BindablePtrs.push_back(std::move(pvs));
		Dynamic::VertexBuffer vbuf(ShaderReflector::GetLayoutFromShader(pvsbc));

		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				dx::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		_BindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));

		_BindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));


		_BindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSSpec.cso"));

		_BindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

		struct PSMaterialConstantDiffuseSpec
		{
			float specularPowerConst;
			BOOL hasGloss;
			float specularMapWeight;
			float padding;
		} pmc;
		pmc.specularPowerConst = shininess;
		pmc.hasGloss = _bHasAlphaGloss ? TRUE : FALSE;
		pmc.specularMapWeight = 1.0f;
		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		_BindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstantDiffuseSpec>::Resolve(gfx, pmc, 1u));
	}
	else if (_bHasDiffuseMap)
	{
		auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
		auto pvsbc = pvs->GetByteCode();
		_BindablePtrs.push_back(std::move(pvs));
		Dynamic::VertexBuffer vbuf(ShaderReflector::GetLayoutFromShader(pvsbc));

		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				dx::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		_BindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));

		_BindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));


		_BindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPS.cso"));

		_BindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

		struct PSMaterialConstantDiffuse
		{
			float specularIntensity;
			float specularPower;
			float padding[2];
		} pmc;
		pmc.specularPower = shininess;
		pmc.specularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		_BindablePtrs.push_back(PixelConstantBuffer<PSMaterialConstantDiffuse>::Resolve(gfx, pmc, 1u));
	}
	else if (!_bHasDiffuseMap && !_bHasNormalMap && !_bHasSpecularMap)
	{
		auto pvs = VertexShader::Resolve(gfx, "PhongVSNotex.cso");
		auto pvsbc = pvs->GetByteCode();
		_BindablePtrs.push_back(std::move(pvs));
		Dynamic::VertexBuffer vbuf(ShaderReflector::GetLayoutFromShader(pvsbc));

		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
		{
			vbuf.EmplaceBack(
				dx::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		_BindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));

		_BindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));


		_BindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSNotex.cso"));

		_BindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));

		Node::PSMaterialConstantNotex pmc;
		pmc.specularPower = shininess;
		pmc.specularColor = specularColor;
		pmc.materialColor = diffuseColor;
		// this is CLEARLY an issue... all meshes will share same mat const, but may have different
		// Ns (specular power) specified for each in the material properties... bad conflict
		_BindablePtrs.push_back(PixelConstantBuffer<Node::PSMaterialConstantNotex>::Resolve(gfx, pmc, 1u));
	}
	else
	{
		throw std::runtime_error("terrible combination of textures in material smh");
	}

	// anything with alpha diffuse is 2-sided IN SPONZA, need a better way
	// of signalling 2-sidedness to be more general in the future
	_BindablePtrs.push_back(Rasterizer::Resolve(gfx, _bHasAlphaDiffuse));

	_BindablePtrs.push_back(Blender::Resolve(gfx, false));

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

	m_pRoot->Draw(gfx, dx::XMLoadFloat4x4(&m_Transform));
}