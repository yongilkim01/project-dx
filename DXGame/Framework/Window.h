#pragma once
#include "DxWin.h"
#include "DxException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"

#include <optional>
#include <memory>

// 윈도우 창을 나타내는 클래스
class Window 
{
public:
	class Exception : public DxException
	{
	public:
		Exception(int, const char*, HRESULT) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept;
		static std::string TranslateErrorCode(HRESULT);
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hr;
	};
private:
	class WindowClass {
	public: 
		static const char* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "Direct3D Engine Window";
		static WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(int, int, const char*);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::string& _title);
	static std::optional<int> ProcessMessages();
	Graphics& Graphic();
private:
	static LRESULT CALLBACK HandleMsgSetup(HWND, UINT, WPARAM, LPARAM) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND, UINT, WPARAM, LPARAM) noexcept;
	LRESULT HandleMsg(HWND, UINT, WPARAM, LPARAM) noexcept;
public:
	Keyboard keybd;
	Mouse mouse;
private:
	int width;
	int height;
	HWND hWnd;
	std::unique_ptr<Graphics> pGraphic;
};

// error exception helper macro
#define DXWND_EXCEPT(hr) Window::Exception(__LINE__,__FILE__,hr)
#define DXWND_LAST_EXCEPT() Window::Exception(__LINE__,__FILE__,GetLastError())