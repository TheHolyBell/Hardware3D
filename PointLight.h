#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"
#include "ConditionalNoexcept.h"
#include "Camera.h"

namespace RenderGraph
{
	class RenderGraph;
}

class PointLight
{
public:
	PointLight(Graphics& gfx, DirectX::XMFLOAT3 pos = {10.0f, 9.0f, 2.5f}, float radius = 0.5f);

	void SpawnControlWindow(Graphics& gfx) noexcept;
	void Reset() noexcept;
	void Submit(size_t channels) const noxnd;
	void Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept;
	void LinkTechniques(RenderGraph::RenderGraph& renderGraph);
	std::shared_ptr<Camera> ShareCamera() const noexcept;
private:
	struct PointLightCBuf
	{
		alignas(16) DirectX::XMFLOAT3 pos;
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16) DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity;
		float attConst;
		float attLin;
		float attQuad;
	};
private:
	bool m_bGuizmoEnabled = false;
	PointLightCBuf m_cbData;
	PointLightCBuf m_Home;
	mutable SolidSphere m_Mesh;
	mutable Bind::PixelConstantBuffer<PointLightCBuf> m_CBuffer;
	std::shared_ptr<Camera> m_pCamera;
};