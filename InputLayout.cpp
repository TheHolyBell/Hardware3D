#include "InputLayout.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	InputLayout::InputLayout(Graphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pVertexShaderBytecode)
	{
		INFOMAN(gfx);

		GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(layout.data(), layout.size(), pVertexShaderBytecode->GetBufferPointer(),
			pVertexShaderBytecode->GetBufferSize(), &m_pInputLayout));
	}

	void InputLayout::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->IASetInputLayout(m_pInputLayout.Get());
	}
}