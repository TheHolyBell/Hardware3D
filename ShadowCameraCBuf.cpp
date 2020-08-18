#include "ShadowCameraCBuf.h"
#include "Camera.h"

namespace Bind
{
	ShadowCameraCBuf::ShadowCameraCBuf(Graphics& gfx, UINT slot)
		: m_pVCBuffer(std::make_unique<VertexConstantBuffer<Transform>>(gfx, slot))
	{
	}
	void ShadowCameraCBuf::Bind(Graphics& gfx) noxnd
	{
		m_pVCBuffer->Bind(gfx);
	}
	void ShadowCameraCBuf::Update(Graphics& gfx)
	{
		const Transform t{
			DirectX::XMMatrixTranspose(
				m_pCamera->GetMatrix() * m_pCamera->GetProjection())};
		m_pVCBuffer->Update(gfx, t);
	}
	void ShadowCameraCBuf::SetCamera(const Camera* p) noexcept
	{
		m_pCamera = p;
	}
}
