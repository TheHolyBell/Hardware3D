#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"
#include "ConditionalNoexcept.h"

namespace RenderGraph
{
	class RenderGraph;
}

class PointLight
{
public:
	PointLight(Graphics& gfx, float radius = 0.5f);

	void SpawnControlWindow(Graphics& gfx) noexcept;
	void Reset() noexcept;
	void Submit() const noxnd;
	void Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept;
	void LinkTechniques(RenderGraph::RenderGraph& renderGraph);
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
	mutable SolidSphere m_Mesh;
	mutable Bind::PixelConstantBuffer<PointLightCBuf> m_CBuffer;
};