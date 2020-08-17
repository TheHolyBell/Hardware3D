#pragma once
#include "Graphics.h"
#include <string>
#include "Projection.h"
#include "CameraIndicator.h"

class Camera
{
public:
	Camera(Graphics& gfx, const std::string& name, DirectX::XMFLOAT3 homePos = {  }, float homePitch = 0.0f, float homeYaw = 0.0f, bool tethered = false) noexcept;
	void BindToGraphics(Graphics& gfx) const;

	DirectX::XMMATRIX GetMatrix() const noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	
	void SpawnControlWidgets(Graphics& gfx) noexcept;
	void Reset(Graphics& gfx) noexcept;
	void Rotate(float dx, float dy) noexcept;
	void Translate(DirectX::XMFLOAT3 translation) noexcept;
	DirectX::XMFLOAT3 GetPos() const noexcept;
	void SetPos(const DirectX::XMFLOAT3& pos) noexcept;
	const std::string& GetName() const noexcept;
	void LinkTechniques(RenderGraph::RenderGraph& rg);
	void Submit(size_t channel) const;
private:
	bool m_bTethered;
	std::string m_Name;
	DirectX::XMFLOAT3 m_HomePos;
	float m_HomePitch;
	float m_HomeYaw;
	DirectX::XMFLOAT3 m_Pos;
	float m_Pitch;
	float m_Yaw;
	static constexpr float s_TravelSpeed = 12.0f;
	static constexpr float s_RotationSpeed = 0.004f;
	bool m_bEnableCameraIndicator = true;
	bool m_bEnableFrustumIndicator = true;
	Projection m_Projection;
	CameraIndicator m_Indicator;
};