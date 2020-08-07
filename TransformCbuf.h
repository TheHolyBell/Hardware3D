#pragma once
#include "Drawable.h"
#include <DirectXMath.h>
#include <memory>
#include "ConstantBuffers.h"
#include "BindableCommon.h"

namespace Bind
{
	class TransformCbuf : public Bindable
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX model;
			DirectX::XMMATRIX MVP;
		};
	public:
		TransformCbuf(Graphics& gfx, UINT slot = 0u);

		virtual void InitializeParentReference(const Drawable& parent) noexcept override;
		
		virtual void Bind(Graphics& gfx) noexcept override;
	protected:
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept;
		Transforms GetTransforms(Graphics& gfx) noexcept;
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> s_pVCBuffer;
		const Drawable* m_pParent;
	};
}