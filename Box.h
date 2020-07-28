#pragma once
#include "TestObject.h"
#include "ConstantBuffers.h"

class Box : public TestObject<Box>
{
public:
	Box(Graphics& gfx, std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist,
		std::uniform_real_distribution<float>& bdist,
		DirectX::XMFLOAT3 material);

	virtual DirectX::XMMATRIX GetTransformXM() const noexcept override;
	// Returns false if window is closed
	bool SpawnControlWindow(int id, Graphics& gfx) noexcept;
private:
	void SyncMaterial(Graphics& gfx) noexcept(!IS_DEBUG);
private:
	struct PSMaterialConstant
	{
		DirectX::XMFLOAT3 color;
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	} m_MaterialConstants;
	using MaterialCbuf = PixelConstantBuffer<PSMaterialConstant>;

	DirectX::XMFLOAT3X3 m_Matrix;
};