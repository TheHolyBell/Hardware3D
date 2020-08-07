#include "SolidSphere.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "Sphere.h"
#include "Vertex.h"
#include "Stencil.h"
#include "ImGui\imgui.h"
#include "ImGui\ImGuizmo.h"

SolidSphere::SolidSphere(Graphics& gfx, float radius)
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Sphere::Make();
	model.Transform(dx::XMMatrixScaling(radius, radius, radius));
	const auto geometryTag = "$sphere." + std::to_string(radius);
	m_pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
	m_pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
	m_pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		Technique solid;
		Step only{ 0 };

		auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
		auto pvsbc = pvs->GetByteCode();
		only.AddBindable(std::move(pvs));

		only.AddBindable(PixelShader::Resolve(gfx, "Solid_PS.cso"));

		struct PSColorConstant
		{
			alignas(16) dx::XMFLOAT3 color = { 1.0f, 1.0f, 1.0f };
		} colorConst;
		only.AddBindable(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 1));

		only.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

		only.AddBindable(std::make_shared<TransformCbuf>(gfx));

		only.AddBindable(Blender::Resolve(gfx, false));

		only.AddBindable(Rasterizer::Resolve(gfx, false));

		solid.AddStep(std::move(only));

		AddTechnique(std::move(solid));
	}

	DirectX::XMStoreFloat4x4(&m_Transformation, DirectX::XMMatrixTranslation(1.0f, 5.0f, 1.0f));
}

void SolidSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	m_Transformation._41 = pos.x;
	m_Transformation._42 = pos.y;
	m_Transformation._43 = pos.z;
}

DirectX::XMFLOAT3 SolidSphere::GetPos() const noexcept
{
	return *(DirectX::XMFLOAT3*)(&m_Transformation._41);
}

void SolidSphere::RenderGizmo(Graphics& gfx) noexcept
{
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	if (ImGui::IsKeyPressed(90))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82)) // r Key
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
	ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<const float*>(m_Transformation.m), matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Translation", matrixTranslation, 3);
	ImGui::InputFloat3("Rotation", matrixRotation, 3);
	ImGui::InputFloat3("Scaling", matrixScale, 3);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, reinterpret_cast<float*>(m_Transformation.m));

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}
	static bool useSnap(false);
	/*if (ImGui::IsKeyPressed(83))
		useSnap = !useSnap;
	ImGui::Checkbox("", &useSnap);*/
	//ImGui::SameLine();
	DirectX::XMFLOAT3 snap;
	
	/*switch (mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		snap = config.mSnapTranslation;
		ImGui::InputFloat3("Snap", &snap.x);
		break;
	case ImGuizmo::ROTATE:
		snap = config.mSnapRotation;
		ImGui::InputFloat("Angle Snap", &snap.x);
		break;
	case ImGuizmo::SCALE:
		snap = config.mSnapScale;
		ImGui::InputFloat("Scale Snap", &snap.x);
		break;
	}*/
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	DirectX::XMFLOAT4X4 _ViewMatrix;
	DirectX::XMFLOAT4X4 _ProjectionMatrix;

	DirectX::XMStoreFloat4x4(&_ViewMatrix, gfx.GetCamera());
	DirectX::XMStoreFloat4x4(&_ProjectionMatrix, gfx.GetProjection());

	ImGuizmo::Manipulate(reinterpret_cast<const float*>(_ViewMatrix.m), reinterpret_cast<const float*>(_ProjectionMatrix.m), mCurrentGizmoOperation, mCurrentGizmoMode, reinterpret_cast<float*>(m_Transformation.m), NULL, useSnap ? &snap.x : NULL);

	if(ImGui::Begin("Debug"))
	{
		ImGui::Text("%f %f %f %f", m_Transformation._11, m_Transformation._12, m_Transformation._13, m_Transformation._14);
		ImGui::Text("%f %f %f %f", m_Transformation._21, m_Transformation._22, m_Transformation._23, m_Transformation._24);
		ImGui::Text("%f %f %f %f", m_Transformation._31, m_Transformation._32, m_Transformation._33, m_Transformation._34);
		ImGui::Text("%f %f %f %f", m_Transformation._41, m_Transformation._42, m_Transformation._43, m_Transformation._44);
	}
	ImGui::End();
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&m_Transformation);
}
