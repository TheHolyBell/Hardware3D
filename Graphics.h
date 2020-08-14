#pragma once
#include "ChiliWin.h"
#include "ChiliException.h"
#include "DxgiInfoManager.h"
#include <d3d11.h>
#include <wrl\client.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <memory>
#include "ConditionalNoexcept.h"
//#include "RenderTarget.h"
//#include "DepthStencil.h"

namespace Bind
{
	class Bindable;
	class RenderTarget;
	class DepthStencil;
}

class Graphics
{
	friend class GraphicsResource;
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

	void EndFrame();
	void BeginFrame(float red, float green, float blue) noexcept;
	
	void DrawIndexed(UINT count) noxnd;
	
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	
	void SetCamera(DirectX::FXMMATRIX cam) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;

	void EnableImGui() noexcept;
	void DisableImGui() noexcept;
	bool IsImGuiEnabled() const noexcept;

	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;
	
	std::shared_ptr<Bind::RenderTarget> GetTarget();
private:
	void OnResize(int Width, int Height);
private:
	bool m_bImGuiEnabled = true;
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif
	UINT m_Width;
	UINT m_Height;

	DirectX::XMMATRIX m_Camera;
	DirectX::XMMATRIX m_Projection;

	Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pImmediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
	std::shared_ptr<Bind::RenderTarget> m_pTarget;
};