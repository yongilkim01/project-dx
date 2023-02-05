#include "Window.h"
#include <sstream>

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(nullptr)) // nullptr�� �����ϸ� �ҽ��� �ۼ��ϰ� �ִ� ���� Module ���� ��ȯ
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
	// ������ â ũ�� ����
	RECT _wndRect;
	_wndRect.left = 100;
	_wndRect.right = _width + _wndRect.left;
	_wndRect.top = 100;
	_wndRect.bottom = _height + _wndRect.top;
	AdjustWindowRect(&_wndRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	// ������ ����
	hWnd = CreateWindow(
		WindowClass::GetName(), _name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, _wndRect.right - _wndRect.left, _wndRect.bottom - _wndRect.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);

	// ������ ����
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
		// ���� �����͸� ptr�� ���� �� Window class�� ���
		const CREATESTRUCTW* const _pCreate = reinterpret_cast<CREATESTRUCTW*>(_lParam); // lParam�� CREATESTRUCTW Ÿ������ ����
		Window* const _pWnd = static_cast<Window*>(_pCreate->lpCreateParams); // Window ����ü �������� Ÿ������ ����
		// Window class�� WinAPI-managed user data�� ����
		SetWindowLongPtr(_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(_pWnd));
		// ������ �Ϸ�Ǿ����Ƿ� �޽��� ���ν����� �Ϲ� ó����� ����
		SetWindowLongPtr(_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		// Window class handler�� �޽����� ����
		return _pWnd->HandleMsg(_hWnd, _msg, _wParam, _lParam);
	}

	// ���� WM_NCCREATE �޽����� �ƴ� �ٸ� �޽����� �Դٸ� �Ϲ� �ڵ鷯�� ó��
	return DefWindowProc(_hWnd, _msg, _wParam, _lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam) noexcept
{
	// Window class�� �ּҸ� ��ȯ
	Window* const _pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(_hWnd, GWLP_USERDATA));
	// Window class handler�� �޽����� ����
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