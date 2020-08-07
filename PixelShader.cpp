#include "PixelShader.h"
#include "GraphicsThrowMacros.h"
#include "BindableCodex.h"
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

namespace Bind
{
	PixelShader::PixelShader(Graphics& gfx, const std::string& path)
		:
		m_Path(path)
	{
		INFOMAN(gfx);

		//Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		GFX_THROW_INFO(D3DReadFileToBlob(std::wstring{ path.begin(),path.end() }.c_str(), &m_pByteCode));
		GFX_THROW_INFO(GetDevice(gfx)->CreatePixelShader(m_pByteCode->GetBufferPointer(), m_pByteCode->GetBufferSize(), nullptr, &m_pPixelShader));
	}

	void PixelShader::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetShader(m_pPixelShader.Get(), nullptr, 0u);
	}
	std::shared_ptr<PixelShader> PixelShader::Resolve(Graphics& gfx, const std::string& path)
	{
		return Codex::Resolve<PixelShader>(gfx, path);
	}
	std::string PixelShader::GenerateUID(const std::string& path)
	{
		using namespace std::string_literals;
		return typeid(PixelShader).name() + "#"s + path;
	}
	std::string PixelShader::GetUID() const noexcept
	{
		return GenerateUID(m_Path);
	}
	ID3DBlob* PixelShader::GetByteCode() const noexcept
	{
		return m_pByteCode.Get();
	}
}