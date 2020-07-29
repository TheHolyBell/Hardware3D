#include "VertexShader.h"
#include "GraphicsThrowMacros.h"

#include <d3dcompiler.h>

namespace Bind
{
	VertexShader::VertexShader(Graphics& gfx, const std::wstring& path)
	{
		INFOMAN(gfx);

		GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &m_pByteCode));
		GFX_THROW_INFO(GetDevice(gfx)->CreateVertexShader(
			m_pByteCode->GetBufferPointer(),
			m_pByteCode->GetBufferSize(),
			nullptr,
			&m_pVertexShader
		));
	}

	void VertexShader::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->VSSetShader(m_pVertexShader.Get(), nullptr, 0u);
	}

	ID3DBlob* VertexShader::GetByteCode() const noexcept
	{
		return m_pByteCode.Get();
	}
}