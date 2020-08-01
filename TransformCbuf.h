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
		TransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot = 0u);
		virtual void Bind(Graphics& gfx) noexcept override;
	protected:
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept;
		Transforms GetTransforms(Graphics& gfx) noexcept;
	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> s_pVCBuffer;
		const Drawable& m_Parent;
	};
}