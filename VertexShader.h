#pragma once
#include "Bindable.h"

class VertexShader : public Bindable
{
public:
	VertexShader(Graphics& gfx, const std::wstring& path);
	virtual void Bind(Graphics& gfx) noexcept override;
	ID3DBlob* GetByteCode() const noexcept;
protected:
	Microsoft::WRL::ComPtr<ID3DBlob> m_pByteCode;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
};