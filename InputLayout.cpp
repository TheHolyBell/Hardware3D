#include "InputLayout.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include "Vertex.h"

namespace Bind
{
	InputLayout::InputLayout(Graphics& gfx,
		Dynamic::VertexLayout layout_in,
		ID3DBlob* pVertexShaderBytecode)
		:
		m_Layout(std::move(layout_in))
	{
		INFOMAN(gfx);

		const auto d3dLayout = m_Layout.GetD3DLayout();

		GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
			d3dLayout.data(), (UINT)d3dLayout.size(),
			pVertexShaderBytecode->GetBufferPointer(),
			pVertexShaderBytecode->GetBufferSize(),
			&m_pInputLayout
		));
	}

	void InputLayout::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->IASetInputLayout(m_pInputLayout.Get());
	}
	std::shared_ptr<InputLayout> InputLayout::Resolve(Graphics& gfx,
		const Dynamic::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode)
	{
		return Codex::Resolve<InputLayout>(gfx, layout, pVertexShaderBytecode);
	}
	std::string InputLayout::GenerateUID(const Dynamic::VertexLayout& layout, ID3DBlob* pVertexShaderBytecode)
	{
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + layout.GetCode();
	}
	std::string InputLayout::GetUID() const noexcept
	{
		return GenerateUID(m_Layout);
	}
}