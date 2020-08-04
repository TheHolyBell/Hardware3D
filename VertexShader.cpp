#include "VertexShader.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include "ShaderReflector.h"

#include <d3dcompiler.h>

namespace Bind
{
	VertexShader::VertexShader(Graphics& gfx, const std::string& path)
		: m_Path(path)
	{
		INFOMAN(gfx);

		GFX_THROW_INFO(D3DReadFileToBlob(std::wstring(path.begin(), path.end()).c_str(), &m_pByteCode));
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

	std::string VertexShader::GetUID() const noexcept
	{
		return GenerateUID(m_Path);
	}

	ID3DBlob* VertexShader::GetByteCode() const noexcept
	{
		return m_pByteCode.Get();
	}

	std::shared_ptr<VertexShader> VertexShader::Resolve(Graphics& gfx, const std::string& path)
	{
		return Codex::Resolve<VertexShader>(gfx, path);
	}
	std::string VertexShader::GenerateUID(const std::string& path)
	{
		using namespace std::string_literals;
		return typeid(VertexShader).name() + "#"s + path;
	}
}