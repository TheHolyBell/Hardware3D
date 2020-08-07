#include "TestPlane.h"
#include "Plane.h"
#include "BindableCommon.h"
#include "TransformCbufDoubleboi.h"
#include "ImGui\imgui.h"
#include "ImGui\ImGuizmo.h"

TestPlane::TestPlane(Graphics& gfx, float size, DirectX::XMFLOAT4 color)
	: m_Pmc({color})
{
	/*using namespace Bind;
	namespace dx = DirectX;

	auto model = Plane::Make();
	model.Transform(dx::XMMatrixScaling(size, size, 1.0f));
	const auto geometryTag = "$plane." + std::to_string(size);
	AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
	AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));

	auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
	auto pvsbc = pvs->GetByteCode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "SolidPS.cso"));

	AddBind(std::make_shared<PixelConstantBuffer<PSMaterialConstant>>(gfx, m_Pmc, 1));

	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));

	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	AddBind(std::make_shared<TransformCbuf>(gfx, *this));

	AddBind(Blender::Resolve(gfx, true, 0.5f));

	AddBind(Rasterizer::Resolve(gfx, true));

	DirectX::XMStoreFloat4x4(&m_Transform, DirectX::XMMatrixIdentity());*/
}

void TestPlane::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	m_Transform._41 = pos.x;
	m_Transform._42 = pos.y;
	m_Transform._43 = pos.z;
}

void TestPlane::SetRotation(float pitch, float yaw, float roll) noexcept
{
	DirectX::XMFLOAT4X4 _RotMatrix;
	DirectX::XMStoreFloat4x4(&_RotMatrix,DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll));

	m_Transform._11 = _RotMatrix._11;
	m_Transform._12 = _RotMatrix._12;
	m_Transform._13 = _RotMatrix._13;

	m_Transform._21 = _RotMatrix._21;
	m_Transform._22 = _RotMatrix._22;
	m_Transform._23 = _RotMatrix._23;

	m_Transform._31 = _RotMatrix._31;
	m_Transform._32 = _RotMatrix._32;
	m_Transform._33 = _RotMatrix._33;
}

DirectX::XMMATRIX TestPlane::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&m_Transform);
}

void TestPlane::SpawnControlWindow(Graphics& gfx, const std::string& name) noexcept
{
	if (ImGui::Begin("Plane"))
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
		ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<const float*>(m_Transform.m), matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("Translation", matrixTranslation, 3);
		ImGui::InputFloat3("Rotation", matrixRotation, 3);
		ImGui::InputFloat3("Scaling", matrixScale, 3);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, reinterpret_cast<float*>(m_Transform.m));

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

		if (m_bGizmoEnabled)
		{
			DirectX::XMFLOAT4X4 _ViewMatrix;
			DirectX::XMFLOAT4X4 _ProjectionMatrix;

			DirectX::XMStoreFloat4x4(&_ViewMatrix, gfx.GetCamera());
			DirectX::XMStoreFloat4x4(&_ProjectionMatrix, gfx.GetProjection());

			ImGuizmo::Manipulate(reinterpret_cast<const float*>(_ViewMatrix.m), reinterpret_cast<const float*>(_ProjectionMatrix.m), mCurrentGizmoOperation, mCurrentGizmoMode, reinterpret_cast<float*>(m_Transform.m), nullptr, nullptr);
		}

		ImGui::Checkbox("Gizmo", &m_bGizmoEnabled);

		/*ImGui::Text("Shading");
		auto pBlender = QueryBindable<Bind::Blender>();
		float factor = pBlender->GetFactor();
		ImGui::SliderFloat("Translucency", &factor, 0.0f, 1.0f);
		pBlender->SetFactor(factor);*/
	}

	ImGui::End();
}
