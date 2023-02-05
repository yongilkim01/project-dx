#include "Window.h"
#include <sstream>

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(nullptr)) // nullptr을 전달하면 소스를 작성하고 있는 곳의 Module 값을 반환
{
	WNDCLASSEX _wc = { 0 };
	_wc.cbSize = sizeof(_wc);
	_wc.style = CS_OWNDC;
	_wc.lpfnWndProc = HandleMsgSetup;
	_wc.cbClsExtra = 0;
	_wc.cbWndExtra = 0;
	_wc.hInstance = GetInstance();
	_wc.hIcon = nullptr;
	_wc.hCursor = nullptr;
	_wc.hbrBackground = nullptr;
	_wc.lpszMenuName = nullptr;
	_wc.lpszClassName = GetName();
	_wc.hIconSm = nullptr;
	RegisterClassEx(&_wc);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

Window::Window(int _width = 0, int _height = 0, const char* _name = "") noexcept
{
	// 윈도우 창 크기 설정
	RECT _wndRect;
	_wndRect.left = 100;
	_wndRect.right = _width + _wndRect.left;
	_wndRect.top = 100;
	_wndRect.bottom = _height + _wndRect.top;
	AdjustWindowRect(&_wndRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	// 윈도우 생성
	hWnd = CreateWindow(
		WindowClass::GetName(), _name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, _wndRect.right - _wndRect.left, _wndRect.bottom - _wndRect.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);

	// 윈도우 띄우기
	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

LRESULT WINAPI Window::HandleMsgSetup(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam) noexcept
{
	if (_msg == WM_NCCREATE) 
	{
		// 생성 데이터를 ptr을 추출 후 Window class에 등록
		const CREATESTRUCTW* const _pCreate = reinterpret_cast<CREATESTRUCTW*>(_lParam); // lParam을 CREATESTRUCTW 타입으로 변경
		Window* const _pWnd = static_cast<Window*>(_pCreate->lpCreateParams); // Window 구조체 포인터의 타입으로 변경
		// Window class에 WinAPI-managed user data를 저장
		SetWindowLongPtr(_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(_pWnd));
		// 설정이 완료되었으므로 메시지 프로시저를 일반 처리기로 설정
		SetWindowLongPtr(_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		// Window class handler에 메시지를 전달
		return _pWnd->HandleMsg(_hWnd, _msg, _wParam, _lParam);
	}

	// 만약 WM_NCCREATE 메시지가 아닌 다른 메시지로 왔다면 일반 핸들러로 처리
	return DefWindowProc(_hWnd, _msg, _wParam, _lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam) noexcept
{
	// Window class의 주소를 반환
	Window* const _pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(_hWnd, GWLP_USERDATA));
	// Window class handler에 메시지를 전달
	return _pWnd->HandleMsg(_hWnd, _msg, _wParam, _lParam);
}

LRESULT Window::HandleMsg(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam) noexcept
{
	switch (_msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}
	
	return DefWindowProc(_hWnd, _msg, _wParam, _lParam);
}

// Window Exception
Window::Exception::Exception(int _line, const char* _file, HRESULT _hr) noexcept
	:
	DxException(_line, _file),
	hr(_hr)
{}

const char* Window::Exception::what() const noexcept
{
	std::ostringstream _oss;
	_oss << GetType() << std::endl
		<< "[Error Code] " << GetErrorCode() << std::endl
		<< "[Description] " << GetErrorString() << std::endl
		<< GetOriginString();
	
	whatBuffer = _oss.str();
	return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept
{
	return "Direct 3D Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT _hr)
{
	char* _pMsgBuf = nullptr;
	DWORD _nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, _hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&_pMsgBuf), 0, nullptr
	);

	if (_nMsgLen == 0)
	{
		return "Unidentified error code";
	}
	std::string _errorString = _pMsgBuf;
	LocalFree(_pMsgBuf);
	return _errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}