#pragma once
#include "ChiliWin.h"
#include "ChiliException.h"
#include "DxgiInfoManager.h"
#include <d3d11.h>
#include <wrl\client.h>
#include <vector>
#include <string>

class Graphics
{
public:
	class Exception : public ChiliException
	{
		using ChiliException::ChiliException;
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		virtual const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT m_HR;
		std::string m_Info;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		virtual const char* GetType() const noexcept override;
	private:
		std::string m_Reason;
	};
public:
	Graphics(HWND hwnd, int Width, int Height);

	Graphics(const Graphics& rhs) = delete;
	Graphics&operator=(const Graphics& rhs) = delete;

	Graphics(Graphics&& rhs) = delete;
	Graphics& operator=(Graphics&& rhs) = delete;
	void ClearBuffer(float red, float green, float blue) noexcept;
	void EndFrame();
private:
#ifndef NDEBUG
	DxgiInfoManager m_InfoManager;
#endif
	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pImmediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRTV;
};