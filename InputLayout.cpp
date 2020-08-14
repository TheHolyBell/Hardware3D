#include "InputLayout.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include "Vertex.h"
#include "VertexShader.h"


namespace Bind
{
	InputLayout::InputLayout(Graphics& gfx,
		Dynamic::VertexLayout layout_in,
		const VertexShader& vs)
		:
		m_Layout(std::move(layout_in))
	{
		INFOMAN(gfx);

		m_VertexShaderUID = vs.GetUID();

		const auto d3dLayout = m_Layout.GetD3DLayout();
		const auto pByteCode = vs.GetByteCode();


		GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(
			d3dLayout.data(), (UINT)d3dLayout.size(),
			pByteCode->GetBufferPointer(),
			pByteCode->GetBufferSize(),
			&m_pInputLayout
		));
	}

	void InputLayout::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->IASetInputLayout(m_pInputLayout.Get());
	}
	std::shared_ptr<InputLayout> InputLayout::Resolve(Graphics& gfx,
		const Dynamic::VertexLayout& layout,
		const VertexShader& vs)
	{
		return Codex::Resolve<InputLayout>(gfx, layout, vs);
	}
	std::string InputLayout::GenerateUID(const Dynamic::VertexLayout& layout, const VertexShader& vs)
	{
		using namespace std::string_literals;
		return typeid(InputLayout).name() + "#"s + layout.GetCode() + "#"s + vs.GetUID();
	}
	std::string InputLayout::GetUID() const noexcept
	{
		using namespace std::string_literals;

		return typeid(InputLayout).name() + "#"s + m_Layout.GetCode() + "#"s + m_VertexShaderUID;
	}
	const Dynamic::VertexLayout InputLayout::GetLayout() const noexcept
	{
		return m_Layout;
	}
}