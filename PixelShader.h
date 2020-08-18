#pragma once
#include "Bindable.h"
#include <memory>

namespace Bind
{
	class PixelShader : public Bindable
	{
	public:
		PixelShader(Graphics& gfx, const std::string& path);

		virtual void Bind(Graphics& gfx) noxnd override;
		virtual std::string GetUID() const noexcept override;

		ID3DBlob* GetByteCode() const noexcept;

		static std::shared_ptr<PixelShader> Resolve(Graphics& gfx, const std::string& path);
		static std::string GenerateUID(const std::string& path);
	protected:
		std::string m_Path;
		Microsoft::WRL::ComPtr<ID3DBlob> m_pByteCode;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
	};
}