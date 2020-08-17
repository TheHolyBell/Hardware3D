#pragma once
#include <DirectXMath.h>
#include "Frustum.h"

class Graphics;
namespace RenderGraph
{
	class RenderGraph;
}

class Projection
{
public:
	Projection(Graphics& gfx, float width, float height, float nearZ, float farZ, float fov = 90.0f);
	DirectX::XMMATRIX GetMatrix() const;
	void RenderWidgets(Graphics& gfx);
	void SetPos(DirectX::XMFLOAT3 pos);
	void SetRotation(DirectX::XMFLOAT3 rot);
	void Submit(size_t channel) const;
	void LinkTechniques(RenderGraph::RenderGraph& renderGraph);
	void Reset(Graphics& gfx);
private:
	float m_Width;
	float m_Height;
	float m_NearZ;
	float m_FarZ;
	float m_Fov;
	float m_HomeWidth;
	float m_HomeHeight;
	float m_HomeNearZ;
	float m_HomeFarZ;
	float m_HomeFov;
	Frustum m_Frust;
};