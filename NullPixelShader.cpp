#include "NullPixelShader.h"
#include "BindableCodex.h"

namespace Bind
{
	NullPixelShader::NullPixelShader(Graphics& gfx)
	{
	}
	void NullPixelShader::Bind(Graphics& gfx) noexcept
	{
		GetContext(gfx)->PSSetShader(nullptr, nullptr, 0);
	}

	std::string NullPixelShader::GetUID() const noexcept
	{
		return GenerateUID();
	}

	std::shared_ptr<NullPixelShader> NullPixelShader::Resolve(Graphics& gfx)
	{
		return Codex::Resolve<NullPixelShader>(gfx);
	}

	std::string NullPixelShader::GenerateUID()
	{
		return typeid(NullPixelShader).name();
	}
}
