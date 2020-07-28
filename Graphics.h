#pragma once
#include "ChiliWin.h"
#include "ChiliException.h"
#include "DxgiInfoManager.h"
#include <d3d11.h>
#include <wrl\client.h>
#include <DirectXMath.h>
#include <vector>
#include <string>

class Graphics
{
	friend class Bindable;
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
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		virtual const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
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
	Graphics(class Window& window);

	Graphics(const Graphics& rhs) = delete;
	Graphics&operator=(const Graphics& rhs) = delete;

	Graphics(Graphics&& rhs) = delete;
	Graphics& operator=(Graphics&& rhs) = delete;
	void BeginFrame(float red, float green, float blue) noexcept;
	void EndFrame();
	void DrawIndexed(UINT count) noexcept(!IS_DEBUG);
	
	void SetProjection(DirectX::FXMMATRIX projection) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	
	void SetCamera(DirectX::FXMMATRIX view) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;

	void EnableImGui() noexcept;
	void DisableImGui() noexcept;
	bool IsImGuiEnabled() const noexcept;
private:
	void OnResize(int Width, int Height);
private:
	bool m_bImGuiEnabled = true;
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif

	DirectX::XMMATRIX m_Camera;
	DirectX::XMMATRIX m_Projection;

	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pImmediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRTV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDSV;
};