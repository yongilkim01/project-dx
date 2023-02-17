#include "resource.h"
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
	_wc.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	_wc.hCursor = nullptr;
	_wc.hbrBackground = nullptr;
	_wc.lpszMenuName = nullptr;
	_wc.lpszClassName = GetName();
	_wc.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
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

Window::Window(int _width, int _height, const char* _name)
	:
	width(_width),
	height(_height)
{
	// ������ â ũ�� ����
	RECT _wndRect;
	_wndRect.left = 100;
	_wndRect.right = _width + _wndRect.left;
	_wndRect.top = 100;
	_wndRect.bottom = _height + _wndRect.top;
	//AdjustWindowRect(&_wndRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	if (AdjustWindowRect(&_wndRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw DXWND_LAST_EXCEPT();
	}

	// ������ ����
	hWnd = CreateWindow(
		WindowClass::GetName(), _name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, _wndRect.right - _wndRect.left, _wndRect.bottom - _wndRect.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);

	if (hWnd == nullptr)
	{
		throw DXWND_LAST_EXCEPT();
	}

	// ������ ����
	ShowWindow(hWnd, SW_SHOWDEFAULT);

	// �׷��� ������Ʈ �����
	pGraphic = std::make_unique<Graphics>(hWnd);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string& _title)
{
	if (SetWindowText(hWnd, _title.c_str()) == 0)
	{
		throw DXWND_LAST_EXCEPT();
	}
}

std::optional<int> Window::ProcessMessages()
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return msg.wParam;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return {};
}

Graphics& Window::Graphic()
{
	return *pGraphic;
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
	/************ KEYBOARD MESSAGES ************/
	case WM_KILLFOCUS:
		keybd.ClearState();
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!(_lParam & 0x40000000) || keybd.AutorepeatIsEnabled())
		{
			keybd.OnKeyPressed(static_cast<unsigned char>(_wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		keybd.OnKeyReleased(static_cast<unsigned char>(_wParam));
		break;
	case WM_CHAR:
		keybd.OnChar(static_cast<unsigned char>(_wParam));
		break;
	/********** END KEYBOARD MESSAGES **********/
	/************* Mouse MESSAGES **************/
	case WM_MOUSEMOVE:
	{
		const POINTS _point = MAKEPOINTS(_lParam);
		if (_point.x >= 0 && _point.x < width &&
			_point.y >= 0 && _point.y < height)
		{
			mouse.OnMouseMove(_point.x, _point.y);
			if (!mouse.IsInWindow()) {
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		else
		{
			if (_wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(_point.x, _point.y);
			}
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
	}
	case WM_LBUTTONDOWN:
	{
		const POINTS _point = MAKEPOINTS(_lParam);
		mouse.OnLeftPressed(_point.x, _point.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		const POINTS _point = MAKEPOINTS(_lParam);
		mouse.OnRightPressed(_point.x, _point.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		const POINTS _point = MAKEPOINTS(_lParam);
		mouse.OnLeftReleased(_point.x, _point.y);
		break;
	}
	case WM_RBUTTONUP:
	{
		const POINTS _point = MAKEPOINTS(_lParam);
		mouse.OnRightReleased(_point.x, _point.y);
		break;
	}
	case WM_MOUSEWHEEL:
	{
		const POINTS _point = MAKEPOINTS(_lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(_wParam);
		mouse.OnWheelDelta(_point.x, _point.y, delta);
		break;
	}
	/*********** END MOUSE MESSAGES ***********/
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