#include "Window.h"

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hInst(GetModuleHandle(nullptr)) // nullptr�� �����ϸ� �ҽ��� �ۼ��ϰ� �ִ� ���� Module ���� ��ȯ
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);
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

Window::Window(int _width, int _height, const char* _name) noexcept 
{
	// ������ â ũ�� ����
	RECT wndRect;
	wndRect.left = 100;
	wndRect.right = _width + wndRect.left;
	wndRect.top = 100;
	wndRect.bottom = _height + wndRect.top;
	AdjustWindowRect(&wndRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	// ������ ����
	hWnd = CreateWindow(
		WindowClass::GetName(), _name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wndRect.right - wndRect.left, wndRect.bottom - wndRect.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);

	// ������ ����
	ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(hWnd);
}

LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (msg == WM_NCCREATE) 
	{
		// ���� �����͸� ptr�� ���� �� Window class�� ���
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam); // lParam�� CREATESTRUCTW Ÿ������ ����
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams); // Window ����ü �������� Ÿ������ ����
		// Window class�� WinAPI-managed user data�� ����
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// ������ �Ϸ�Ǿ����Ƿ� �޽��� ���ν����� �Ϲ� ó����� ����
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
		// Window class handler�� �޽����� ����
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	// ���� WM_NCCREATE �޽����� �ƴ� �ٸ� �޽����� �Դٸ� �Ϲ� �ڵ鷯�� ó��
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	// Window class�� �ּҸ� ��ȯ
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// Window class handler�� �޽����� ����
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	}
	
	return DefWindowProc(hWnd, msg, wParam, lParam);
}