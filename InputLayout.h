#pragma once
#include "Bindable.h"
#include "Vertex.h"
#include <memory>

namespace Bind
{
	class VertexShader;

	class InputLayout : public Bindable
	{
	public:
		InputLayout(Graphics& gfx,
			Dynamic::VertexLayout layout,
			const VertexShader& vs);

		virtual void Bind(Graphics& gfx) noexcept override;
		virtual std::string GetUID() const noexcept override;
		
		const Dynamic::VertexLayout GetLayout() const noexcept;

		static std::shared_ptr<InputLayout> Resolve(Graphics& gfx,
			const Dynamic::VertexLayout& layout, const VertexShader& vs);
		static std::string GenerateUID(const Dynamic::VertexLayout& layout, const VertexShader& vs);
	protected:
		std::string m_VertexShaderUID;
		Dynamic::VertexLayout m_Layout;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
	};
}