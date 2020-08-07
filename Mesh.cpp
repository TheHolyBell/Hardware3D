#include "Mesh.h"
#include "ImGui/imgui.h"
#include "ImGui\ImGuizmo.h"
#include "Surface.h"
#include <unordered_map>
#include <sstream>
#include <filesystem>
#include "ChiliXM.h"
#include "DynamicConstant.h"
#include "ConstantBuffersEx.h"
#include "LayoutCodex.h"
#include "Stencil.h"
#include "ShaderReflector.h"
#include "ModelException.h"

namespace dx = DirectX;

Mesh::Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale) noxnd
	: Drawable(gfx, mat, mesh, scale)
{}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&m_Transform);
}

void Mesh::Submit(FrameCommander& frame, dx::FXMMATRIX accumulatedTransform) const noxnd
{
	dx::XMStoreFloat4x4(&m_Transform, accumulatedTransform);
	Drawable::Submit(frame);
}

//
//// Node
//Node::Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, const DirectX::XMMATRIX& transform_in) noxnd
//	:
//	m_ID(id),
//	m_MeshPtrs(std::move(meshPtrs)),
//	m_Name(name)
//{
//	dx::XMStoreFloat4x4(&m_Transform, transform_in);
//	dx::XMStoreFloat4x4(&m_AppliedTransform, dx::XMMatrixIdentity());
//}
//
//void Node::Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
//{
//	const auto built =
//		dx::XMLoadFloat4x4(&m_AppliedTransform) *
//		dx::XMLoadFloat4x4(&m_Transform) *
//		accumulatedTransform;
//	for (const auto pm : m_MeshPtrs)
//	{
//		pm->Submit(frame, accumulatedTransform);
//	}
//	for (const auto& pc : m_ChildPtrs)
//	{
//		pc->Submit(frame, accumulatedTransform);
//	}
//}
//
//void Node::AddChild(std::unique_ptr<Node> pChild) noxnd
//{
//	assert(pChild);
//	m_ChildPtrs.push_back(std::move(pChild));
//}
//
//void Node::ShowTree(Node*& pSelectedNode) const noexcept
//{
//	// if there is no selected node, set selectedId to an impossible value
//	const int selectedId = (pSelectedNode == nullptr) ? -1 : pSelectedNode->GetId();
//	// build up flags for current node
//	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
//		| ((GetId() == selectedId) ? ImGuiTreeNodeFlags_Selected : 0)
//		| ((m_ChildPtrs.size() == 0) ? ImGuiTreeNodeFlags_Leaf : 0);
//	// render this node
//	const auto expanded = ImGui::TreeNodeEx(
//		(void*)(intptr_t)GetId(), node_flags, m_Name.c_str()
//	);
//	// processing for selecting node
//	if (ImGui::IsItemClicked())
//	{
//		pSelectedNode = const_cast<Node*>(this);
//	}
//	// recursive rendering of open node's children
//	if (expanded)
//	{
//		for (const auto& pChild : m_ChildPtrs)
//		{
//			pChild->ShowTree(pSelectedNode);
//		}
//		ImGui::TreePop();
//	}
//}
//
////const Dcb::Buffer* Node::GetMaterialConstants() const noxnd
////{
////	if( meshPtrs.size() == 0 )
////	{
////		return nullptr;
////	}
////	auto pBindable = meshPtrs.front()->QueryBindable<Bind::CachingPixelConstantBufferEX>();
////	return &pBindable->GetBuffer();
////}
////
////void Node::SetMaterialConstants( const Dcb::Buffer& buf_in ) noxnd
////{
////	auto pcb = meshPtrs.front()->QueryBindable<Bind::CachingPixelConstantBufferEX>();
////	assert( pcb != nullptr );
////	pcb->SetBuffer( buf_in );
////}
//
//void Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
//{
//	dx::XMStoreFloat4x4(&m_AppliedTransform, transform);
//}
//
//const DirectX::XMFLOAT4X4& Node::GetAppliedTransform() const noexcept
//{
//	return m_AppliedTransform;
//}
//
//int Node::GetId() const noexcept
//{
//	return m_ID;
//}
//
//
//
//// Model
//class ModelWindow // pImpl idiom, only defined in this .cpp
//{
//public:
//	void Show(Graphics& gfx, const char* windowName, const Node& root) noexcept
//	{
//		/*// window name defaults to "Model"
//		windowName = windowName ? windowName : "Model";
//		// need an ints to track node indices and selected node
//		int nodeIndexTracker = 0;
//		if (ImGui::Begin(windowName))
//		{
//			ImGui::Columns(2, nullptr, true);
//			root.ShowTree(pSelectedNode);
//
//			ImGui::NextColumn();
//			if (pSelectedNode != nullptr)
//			{
//				const auto id = pSelectedNode->GetId();
//				auto i = transforms.find(id);
//				if (i == transforms.end())
//				{
//					const auto& applied = pSelectedNode->GetAppliedTransform();
//					const auto angles = ExtractEulerAngles(applied);
//					const auto translation = ExtractTranslation(applied);
//					TransformParameters tp;
//					tp.roll = angles.z;
//					tp.pitch = angles.x;
//					tp.yaw = angles.y;
//					tp.x = translation.x;
//					tp.y = translation.y;
//					tp.z = translation.z;
//					auto pMatConst = pSelectedNode->GetMaterialConstants();
//					auto buf = pMatConst != nullptr ? std::optional<Dynamic::Buffer>{ *pMatConst } : std::optional<Dynamic::Buffer>{};
//					std::tie(i, std::ignore) = transforms.insert({ id,{ tp,false,std::move(buf),false } });
//				}
//				// link imgui ctrl to our cached transform params
//				{
//					auto& transform = i->second.tranformParams;
//					// dirty check
//					auto& dirty = i->second.transformParamsDirty;
//					const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
//					// widgets
//					ImGui::Text("Orientation");
//					dcheck(ImGui::SliderAngle("Roll", &transform.roll, -180.0f, 180.0f));
//					dcheck(ImGui::SliderAngle("Pitch", &transform.pitch, -180.0f, 180.0f));
//					dcheck(ImGui::SliderAngle("Yaw", &transform.yaw, -180.0f, 180.0f));
//					ImGui::Text("Position");
//					dcheck(ImGui::SliderFloat("X", &transform.x, -20.0f, 20.0f));
//					dcheck(ImGui::SliderFloat("Y", &transform.y, -20.0f, 20.0f));
//					dcheck(ImGui::SliderFloat("Z", &transform.z, -20.0f, 20.0f));
//				}
//				// link imgui ctrl to our cached material params
//				if (i->second.materialCbuf)
//				{
//					auto& mat = *i->second.materialCbuf;
//					// dirty check
//					auto& dirty = i->second.materialCbufDirty;
//					const auto dcheck = [&dirty](bool changed) {dirty = dirty || changed; };
//					// widgets
//					ImGui::Text("Material");
//					if (auto v = mat["normalMapEnabled"]; v.Exists())
//					{
//						dcheck(ImGui::Checkbox("Norm Map", &v));
//					}
//					if (auto v = mat["specularMapEnabled"]; v.Exists())
//					{
//						dcheck(ImGui::Checkbox("Spec Map", &v));
//					}
//					if (auto v = mat["hasGlossMap"]; v.Exists())
//					{
//						dcheck(ImGui::Checkbox("Gloss Map", &v));
//					}
//					if (auto v = mat["materialColor"]; v.Exists())
//					{
//						dcheck(ImGui::ColorPicker3("Diff Color", reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v))));
//					}
//					if (auto v = mat["specularPower"]; v.Exists())
//					{
//						dcheck(ImGui::SliderFloat("Spec Power", &v, 0.0f, 100.0f, "%.1f", 1.5f));
//					}
//					if (auto v = mat["specularColor"]; v.Exists())
//					{
//						dcheck(ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&static_cast<dx::XMFLOAT3&>(v))));
//					}
//					if (auto v = mat["specularMapWeight"]; v.Exists())
//					{
//						dcheck(ImGui::SliderFloat("Spec Weight", &v, 0.0f, 4.0f));
//					}
//					if (auto v = mat["specularIntensity"]; v.Exists())
//					{
//						dcheck(ImGui::SliderFloat("Spec Intens", &v, 0.0f, 1.0f));
//					}
//				}
//			}
//		}
//		ImGui::End();*/
//	}
//	void ApplyParameters() noxnd
//	{
//		/*if (TransformDirty())
//		{
//			pSelectedNode->SetAppliedTransform(GetTransform());
//			ResetTransformDirty();
//		}
//		if (MaterialDirty())
//		{
//			pSelectedNode->SetMaterialConstants(GetMaterial());
//			ResetMaterialDirty();
//		}*/
//	}
//private:
//	dx::XMMATRIX GetTransform() const noexcept(!IS_DEBUG)
//	{
//		assert(pSelectedNode != nullptr);
//		const auto& transform = transforms.at(pSelectedNode->GetId()).tranformParams;
//		return
//			dx::XMMatrixRotationRollPitchYaw(transform.roll, transform.pitch, transform.yaw) *
//			dx::XMMatrixTranslation(transform.x, transform.y, transform.z);
//	}
//	const Dynamic::Buffer& GetMaterial() const noexcept(!IS_DEBUG)
//	{
//		assert(pSelectedNode != nullptr);
//		const auto& mat = transforms.at(pSelectedNode->GetId()).materialCbuf;
//		assert(mat);
//		return *mat;
//	}
//	bool TransformDirty() const noexcept(!IS_DEBUG)
//	{
//		return pSelectedNode && transforms.at(pSelectedNode->GetId()).transformParamsDirty;
//	}
//	void ResetTransformDirty() noexcept(!IS_DEBUG)
//	{
//		transforms.at(pSelectedNode->GetId()).transformParamsDirty = false;
//	}
//	bool MaterialDirty() const noexcept(!IS_DEBUG)
//	{
//		return pSelectedNode && transforms.at(pSelectedNode->GetId()).materialCbufDirty;
//	}
//	void ResetMaterialDirty() noexcept(!IS_DEBUG)
//	{
//		transforms.at(pSelectedNode->GetId()).materialCbufDirty = false;
//	}
//	bool IsDirty() const noexcept(!IS_DEBUG)
//	{
//		return TransformDirty() || MaterialDirty();
//	}
//private:
//	Node* pSelectedNode;
//	struct TransformParameters
//	{
//		float roll = 0.0f;
//		float pitch = 0.0f;
//		float yaw = 0.0f;
//		float x = 0.0f;
//		float y = 0.0f;
//		float z = 0.0f;
//	};
//	struct NodeData
//	{
//		TransformParameters tranformParams;
//		bool transformParamsDirty;
//		std::optional<Dynamic::Buffer> materialCbuf;
//		bool materialCbufDirty;
//	};
//	std::unordered_map<int, NodeData> transforms;
//};
//
//Model::Model(Graphics& gfx, const std::string& path, float scale)
//	: m_pWindow(std::make_unique<ModelWindow>())
//{
//	Assimp::Importer _Importer;
//	const auto pScene = _Importer.ReadFile(path,
//		aiProcess_Triangulate |
//		aiProcess_JoinIdenticalVertices |
//		aiProcess_ConvertToLeftHanded |
//		aiProcess_GenNormals |
//		aiProcess_CalcTangentSpace
//	);
//
//	if (pScene == nullptr)
//		throw ModelException(__LINE__, __FILE__, _Importer.GetErrorString());
//
//	for (size_t i = 0; i < pScene->mNumMeshes; i++)
//	{
//		m_MeshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials, path, scale));
//	}
//
//
//	int nextId = 0;
//	m_pRoot = ParseNode(nextId, *pScene->mRootNode);
//	DirectX::XMStoreFloat4x4(&m_Transform, DirectX::XMMatrixIdentity());
//}
//
//void Model::Submit(FrameCommander& frame) const noxnd
//{
//	m_pWindow->ApplyParameters();
//	m_pRoot->Submit(frame, DirectX::XMLoadFloat4x4(&m_Transform));
//}
//
//void Model::ShowWindow(Graphics& gfx, const char* windowName) noexcept
//{
//	m_pWindow->Show(gfx, windowName, *m_pRoot);
//	Model* _this = const_cast<Model*>(this);
//
//	if (ImGui::Begin((std::string("Miscelanious: #") + windowName).c_str()))
//	{
//		ImGui::Checkbox("Show Gizmo", &_this->m_bShowGizmo);
//		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
//		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
//		if (ImGui::IsKeyPressed('T'))
//			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
//		if (ImGui::IsKeyPressed('R'))
//			mCurrentGizmoOperation = ImGuizmo::ROTATE;
//		if (ImGui::IsKeyPressed('L')) // r Key
//			mCurrentGizmoOperation = ImGuizmo::SCALE;
//		if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
//			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
//		ImGui::SameLine();
//		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
//			mCurrentGizmoOperation = ImGuizmo::ROTATE;
//		ImGui::SameLine();
//		if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
//			mCurrentGizmoOperation = ImGuizmo::SCALE;
//		float matrixTranslation[3], matrixRotation[3], matrixScale[3];
//		ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<const float*>(m_Transform.m), matrixTranslation, matrixRotation, matrixScale);
//		ImGui::InputFloat3("Translation", matrixTranslation, 3);
//		ImGui::InputFloat3("Rotation", matrixRotation, 3);
//		ImGui::InputFloat3("Scaling", matrixScale, 3);
//		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, reinterpret_cast<float*>(_this->m_Transform.m));
//
//		if (mCurrentGizmoOperation != ImGuizmo::SCALE)
//		{
//			if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
//				mCurrentGizmoMode = ImGuizmo::LOCAL;
//			ImGui::SameLine();
//			if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
//				mCurrentGizmoMode = ImGuizmo::WORLD;
//		}
//
//		ImGuiIO& io = ImGui::GetIO();
//		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
//
//		if (m_bShowGizmo)
//		{
//			DirectX::XMFLOAT4X4 _ViewMatrix;
//			DirectX::XMFLOAT4X4 _ProjectionMatrix;
//
//			DirectX::XMStoreFloat4x4(&_ViewMatrix, gfx.GetCamera());
//			DirectX::XMStoreFloat4x4(&_ProjectionMatrix, gfx.GetProjection());
//
//			ImGuizmo::Manipulate(reinterpret_cast<const float*>(_ViewMatrix.m), reinterpret_cast<const float*>(_ProjectionMatrix.m), mCurrentGizmoOperation, mCurrentGizmoMode, reinterpret_cast<float*>(_this->m_Transform.m), nullptr, nullptr);
//
//		}
//	}
//	ImGui::End();
//}
//
//void Model::SetRootTransform(DirectX::FXMMATRIX transform) noexcept
//{
//	m_pRoot->SetAppliedTransform(transform);
//}
//
//Model::~Model() noexcept
//{
//}
//
//std::unique_ptr<Mesh> Model::ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale)
//{
//	using namespace std::string_literals;
//	using Dynamic::VertexLayout;
//	using namespace Bind;
//
//	std::vector<std::shared_ptr<Bindable>> _BindablePtrs;
//
//	const auto rootPath = path.parent_path().string() + "\\";
//
//	bool _bHasSpecularMap = false;
//	bool _bHasAlphaGloss = false;
//	bool _bHasAlphaDiffuse = false;
//	bool _bHasNormalMap = false;
//	bool _bHasDiffuseMap = false;
//	float shininess = 2.0f;
//	dx::XMFLOAT4 specularColor = { 0.18f,0.18f,0.18f,1.0f };
//	dx::XMFLOAT4 diffuseColor = { 0.45f,0.45f,0.85f,1.0f };
//	if (mesh.mMaterialIndex >= 0)
//	{
//		auto& material = *pMaterials[mesh.mMaterialIndex];
//
//		aiString texFileName;
//
//		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
//		{
//			auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str());
//			_bHasAlphaDiffuse = tex->HasAlpha();
//			_BindablePtrs.push_back(std::move(tex));
//			_bHasDiffuseMap = true;
//		}
//		else
//		{
//			material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(diffuseColor));
//		}
//
//		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
//		{
//			auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 1);
//			_bHasAlphaGloss = tex->HasAlpha();
//			_BindablePtrs.push_back(std::move(tex));
//			_bHasSpecularMap = true;
//		}
//		else
//		{
//			material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(specularColor));
//		}
//		if (!_bHasAlphaGloss)
//		{
//			material.Get(AI_MATKEY_SHININESS, shininess);
//		}
//
//		if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
//		{
//			auto tex = Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 2);
//			_bHasAlphaGloss = tex->HasAlpha();
//			_BindablePtrs.push_back(std::move(tex));
//			_bHasNormalMap = true;
//		}
//
//		if (_bHasDiffuseMap || _bHasSpecularMap || _bHasNormalMap)
//		{
//			_BindablePtrs.push_back(Bind::Sampler::Resolve(gfx));
//		}
//	}
//
//	const auto meshTag = path.string() + "%" + mesh.mName.C_Str();
//
//	if (_bHasDiffuseMap && _bHasNormalMap && _bHasSpecularMap)
//	{
//		auto pvs = VertexShader::Resolve(gfx, "PhongVSNormalMap.cso");
//		auto pvsbc = pvs->GetByteCode();
//		_BindablePtrs.push_back(std::move(pvs));
//		Dynamic::VertexBuffer vbuf(ShaderReflector::GetLayoutFromShader(pvsbc));
//
//		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
//		{
//			vbuf.EmplaceBack(
//				dx::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
//				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
//				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mTangents[i]),
//				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mBitangents[i]),
//				*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
//			);
//		}
//
//		std::vector<unsigned short> indices;
//		indices.reserve(mesh.mNumFaces * 3);
//		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
//		{
//			const auto& face = mesh.mFaces[i];
//			assert(face.mNumIndices == 3);
//			indices.push_back(face.mIndices[0]);
//			indices.push_back(face.mIndices[1]);
//			indices.push_back(face.mIndices[2]);
//		}
//
//		_BindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
//
//		_BindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));
//
//
//		_BindablePtrs.push_back(PixelShader::Resolve(gfx,
//			_bHasAlphaDiffuse ? "PhongPSSpecNormMask.cso" : "PhongPSSpecNormalMap.cso"
//		));
//
//		_BindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));
//
//		Dynamic::RawLayout lay;
//		lay.Add<Dynamic::Bool>("normalMapEnabled");
//		lay.Add<Dynamic::Bool>("specularMapEnabled");
//		lay.Add<Dynamic::Bool>("hasGlossMap");
//		lay.Add<Dynamic::Float>("specularPower");
//		lay.Add<Dynamic::Float3>("specularColor");
//		lay.Add<Dynamic::Float>("specularMapWeight");
//
//		auto buf = Dynamic::Buffer(std::move(lay));
//		buf["normalMapEnabled"] = true;
//		buf["specularMapEnabled"] = true;
//		buf["hasGlossMap"] = _bHasAlphaGloss;
//		buf["specularPower"] = shininess;
//		buf["specularColor"] = dx::XMFLOAT3{ 0.75f,0.75f,0.75f };
//		buf["specularMapWeight"] = 0.671f;
//
//		_BindablePtrs.push_back(std::make_shared<CachingPixelConstantBufferEX>(gfx, buf, 1u));
//	}
//	else if (_bHasDiffuseMap && _bHasNormalMap)
//	{
//		auto pvs = VertexShader::Resolve(gfx, "PhongVSNormalMap.cso");
//		auto pvsbc = pvs->GetByteCode();
//		_BindablePtrs.push_back(std::move(pvs));
//		Dynamic::VertexBuffer vbuf(ShaderReflector::GetLayoutFromShader(pvsbc));
//
//		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
//		{
//			vbuf.EmplaceBack(
//				dx::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
//				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
//				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mTangents[i]),
//				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mBitangents[i]),
//				*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
//			);
//		}
//
//		std::vector<unsigned short> indices;
//		indices.reserve(mesh.mNumFaces * 3);
//		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
//		{
//			const auto& face = mesh.mFaces[i];
//			assert(face.mNumIndices == 3);
//			indices.push_back(face.mIndices[0]);
//			indices.push_back(face.mIndices[1]);
//			indices.push_back(face.mIndices[2]);
//		}
//
//		_BindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
//
//		_BindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));
//
//
//		_BindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSNormalMap.cso"));
//
//		_BindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));
//
//		Dynamic::RawLayout layout;
//		layout.Add<Dynamic::Float>("specularIntensity");
//		layout.Add<Dynamic::Float>("specularPower");
//		layout.Add<Dynamic::Bool>("normalMapEnabled");
//
//		auto cbuf = Dynamic::Buffer(std::move(layout));
//		cbuf["specularIntensity"] = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
//		cbuf["specularPower"] = shininess;
//		cbuf["normalMapEnabled"] = true;
//
//		_BindablePtrs.push_back(std::make_shared<CachingPixelConstantBufferEX>(gfx, cbuf, 1u));
//	}
//	else if (_bHasDiffuseMap && !_bHasNormalMap && _bHasSpecularMap)
//	{
//		auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
//		auto pvsbc = pvs->GetByteCode();
//		_BindablePtrs.push_back(std::move(pvs));
//		Dynamic::VertexBuffer vbuf(ShaderReflector::GetLayoutFromShader(pvsbc));
//
//		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
//		{
//			vbuf.EmplaceBack(
//				dx::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
//				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
//				*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
//			);
//		}
//
//		std::vector<unsigned short> indices;
//		indices.reserve(mesh.mNumFaces * 3);
//		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
//		{
//			const auto& face = mesh.mFaces[i];
//			assert(face.mNumIndices == 3);
//			indices.push_back(face.mIndices[0]);
//			indices.push_back(face.mIndices[1]);
//			indices.push_back(face.mIndices[2]);
//		}
//
//		_BindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
//
//		_BindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));
//
//
//		_BindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSSpec.cso"));
//
//		_BindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));
//
//		Dynamic::RawLayout lay;
//		lay.Add<Dynamic::Float>("specularPower");
//		lay.Add<Dynamic::Bool>("hasGloss");
//		lay.Add<Dynamic::Float>("specularMapWeight");
//
//		auto buf = Dynamic::Buffer(std::move(lay));
//		buf["specularPower"] = shininess;
//		buf["hasGloss"] = _bHasAlphaGloss;
//		buf["specularMapWeight"] = 1.0f;
//
//		_BindablePtrs.push_back(std::make_unique<Bind::CachingPixelConstantBufferEX>(gfx, buf, 1u));
//	}
//	else if (_bHasDiffuseMap)
//	{
//		auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
//		auto pvsbc = pvs->GetByteCode();
//		_BindablePtrs.push_back(std::move(pvs));
//		Dynamic::VertexBuffer vbuf(ShaderReflector::GetLayoutFromShader(pvsbc));
//
//		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
//		{
//			vbuf.EmplaceBack(
//				dx::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
//				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i]),
//				*reinterpret_cast<dx::XMFLOAT2*>(&mesh.mTextureCoords[0][i])
//			);
//		}
//
//		std::vector<unsigned short> indices;
//		indices.reserve(mesh.mNumFaces * 3);
//		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
//		{
//			const auto& face = mesh.mFaces[i];
//			assert(face.mNumIndices == 3);
//			indices.push_back(face.mIndices[0]);
//			indices.push_back(face.mIndices[1]);
//			indices.push_back(face.mIndices[2]);
//		}
//
//		_BindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
//
//		_BindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));
//
//
//		_BindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPS.cso"));
//
//		_BindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));
//
//		Dynamic::RawLayout lay;
//		lay.Add<Dynamic::Float>("specularIntensity");
//		lay.Add<Dynamic::Float>("specularPower");
//		lay.Add<Dynamic::Float>("specularMapWeight");
//
//		auto buf = Dynamic::Buffer(std::move(lay));
//		buf["specularIntensity"] = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
//		buf["specularPower"] = shininess;
//		buf["specularMapWeight"] = 1.0f;
//
//		_BindablePtrs.push_back(std::make_unique<Bind::CachingPixelConstantBufferEX>(gfx, buf, 1u));
//	}
//	else if (!_bHasDiffuseMap && !_bHasNormalMap && !_bHasSpecularMap)
//	{
//		auto pvs = VertexShader::Resolve(gfx, "PhongVSNotex.cso");
//		auto pvsbc = pvs->GetByteCode();
//		_BindablePtrs.push_back(std::move(pvs));
//		Dynamic::VertexBuffer vbuf(ShaderReflector::GetLayoutFromShader(pvsbc));
//
//		for (unsigned int i = 0; i < mesh.mNumVertices; i++)
//		{
//			vbuf.EmplaceBack(
//				dx::XMFLOAT3(mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale),
//				*reinterpret_cast<dx::XMFLOAT3*>(&mesh.mNormals[i])
//			);
//		}
//
//		std::vector<unsigned short> indices;
//		indices.reserve(mesh.mNumFaces * 3);
//		for (unsigned int i = 0; i < mesh.mNumFaces; i++)
//		{
//			const auto& face = mesh.mFaces[i];
//			assert(face.mNumIndices == 3);
//			indices.push_back(face.mIndices[0]);
//			indices.push_back(face.mIndices[1]);
//			indices.push_back(face.mIndices[2]);
//		}
//
//		_BindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
//
//		_BindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));
//
//
//		_BindablePtrs.push_back(PixelShader::Resolve(gfx, "PhongPSNotex.cso"));
//
//		_BindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pvsbc));
//
//		Dynamic::RawLayout lay;
//		lay.Add<Dynamic::Float4>("materialColor");
//		lay.Add<Dynamic::Float4>("specularColor");
//		lay.Add<Dynamic::Float>("specularPower");
//
//		auto buf = Dynamic::Buffer(std::move(lay));
//		buf["specularPower"] = shininess;
//		buf["specularColor"] = specularColor;
//		buf["materialColor"] = diffuseColor;
//
//		_BindablePtrs.push_back(std::make_unique<Bind::CachingPixelConstantBufferEX>(gfx, buf, 1u));
//	}
//	else
//	{
//		throw std::runtime_error("terrible combination of textures in material smh");
//	}
//
//	// anything with alpha diffuse is 2-sided IN SPONZA, need a better way
//	// of signalling 2-sidedness to be more general in the future
//	_BindablePtrs.push_back(Rasterizer::Resolve(gfx, _bHasAlphaDiffuse));
//
//	_BindablePtrs.push_back(Blender::Resolve(gfx, false));
//
//	_BindablePtrs.push_back(std::make_shared<Stencil>(gfx, Stencil::Mode::Off));
//
//	return nullptr;
//	//return std::make_unique<Mesh>(gfx, std::move(_BindablePtrs));
//}
//
//std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node) noexcept
//{
//	namespace dx = DirectX;
//	const auto transform = dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
//		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
//	));
//
//	std::vector<Mesh*> curMeshPtrs;
//	curMeshPtrs.reserve(node.mNumMeshes);
//	for (size_t i = 0; i < node.mNumMeshes; i++)
//	{
//		const auto meshIdx = node.mMeshes[i];
//		curMeshPtrs.push_back(m_MeshPtrs.at(meshIdx).get());
//	}
//
//	auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
//	for (size_t i = 0; i < node.mNumChildren; i++)
//	{
//		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));
//	}
//
//	return pNode;
//}
