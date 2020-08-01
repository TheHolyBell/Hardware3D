#pragma once
#include "Bindable.h"
#include <memory>

namespace Bind
{
	class VertexShader : public Bindable
	{
	public:
		VertexShader(Graphics& gfx, const std::string& path);
		
		ID3DBlob* GetByteCode() const noexcept;
		
		static std::shared_ptr<VertexShader> Resolve(Graphics& gfx, const std::string& path);
		static std::string GenerateUID(const std::string& path);

		virtual void Bind(Graphics& gfx) noexcept override;
		virtual std::string GetUID() const noexcept override;
		
	protected:
		std::string m_Path;
		Microsoft::WRL::ComPtr<ID3DBlob> m_pByteCode;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
	};
}