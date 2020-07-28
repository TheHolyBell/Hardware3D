#pragma once
#include "ChiliWin.h"
#include "ChiliException.h"
#include <optional>
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <memory>
#include <functional>

class Window
{	
public:
	class Exception : public ChiliException
	{
		using ChiliException::ChiliException;
	public:
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
	};

	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr) noexcept;
		
		virtual const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT m_HR;
	};
	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		virtual const char* GetType() const noexcept override;
	};
private:
	// Singleton manages registration/cleanup of window class
	class WindowClass
	{
	public:
		WindowClass(const WindowClass& rhs) = delete;
		WindowClass& operator=(const WindowClass& rhs) = delete;

		WindowClass(WindowClass&& rhs) = delete;
		WindowClass& operator=(WindowClass&& rhs) = delete;
		
		~WindowClass();

		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		HINSTANCE m_hInstance;
		static WindowClass s_Instance;
		static constexpr const char* s_WndClassName = "Direct3D Engine Window";
	};

public:
	Window(int Width, int Height, const char* Name);
	~Window();

	Window(const Window& rhs) = delete;
	Window&operator=(const Window& rhs) = delete;
	
	Window(Window&& rhs) = delete;
	Window&operator=(Window&& rhs) = delete;

	void RegisterOnResizeCallback(std::function<void(int, int)> Func);

	void SetTitle(const std::string& title);
	Graphics& Gfx() const;
	
	HWND GetHWND() const;
	int GetWidth() const;
	int GetHeight() const;
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	Keyboard g_Keyboard;
	Mouse g_Mouse;
private:
	int m_Width;
	int m_Height;
	HWND m_hWnd;
	std::unique_ptr<Graphics> m_pGraphics;
	
	std::function<void(int, int)> OnResize;
};