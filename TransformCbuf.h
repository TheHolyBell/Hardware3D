#pragma once
#include "Drawable.h"
#include <DirectXMath.h>
#include <memory>
#include "ConstantBuffers.h"
#include "BindableCommon.h"

namespace Bind
{
	class TransformCbuf : public CloningBindable
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX model;
			DirectX::XMMATRIX modelView;
			DirectX::XMMATRIX MVP;
		};
	public:
		TransformCbuf(Graphics& gfx, UINT slot = 0u);
		void Bind(Graphics& gfx) noexcept override;
		void InitializeParentReference(const Drawable& parent) noexcept override;
		std::unique_ptr<CloningBindable> Clone() const noexcept override;
	protected:
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noxnd;
		Transforms GetTransforms(Graphics& gfx) noxnd;
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> s_pVCBuffer;
		const Drawable* m_pParent;
	};
}