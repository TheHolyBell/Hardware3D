#pragma once
#include "Bindable.h"
#include "Vertex.h"
#include <memory>

namespace Bind
{
	class InputLayout : public Bindable
	{
	public:
		InputLayout(Graphics& gfx,
			Dynamic::VertexLayout layout,
			ID3DBlob* pVertexShaderByteCode);

		virtual void Bind(Graphics& gfx) noexcept override;
		virtual std::string GetUID() const noexcept override;

		static std::shared_ptr<InputLayout> Resolve(Graphics& gfx,
			const Dynamic::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode);
		static std::string GenerateUID(const Dynamic::VertexLayout& layout, ID3DBlob* pVertexShaderByteCode = nullptr);
	protected:
		Dynamic::VertexLayout m_Layout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
	};
}