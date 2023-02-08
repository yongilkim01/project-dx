#include "Window.h"
#include <sstream>

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow) 
{
	try
	{
		Window wnd(800, 300, "My Direct3D");

		MSG msg;
		BOOL gResult;

		while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			
			// do app logic (test)
			std::ostringstream _oss;
			while (!wnd.mouse.IsEmpty())
			{
				const auto _e = wnd.mouse.Read();
				switch (_e.GetType())
				{
				case Mouse::Event::Type::WheelUp:
					_oss << "Up : ";
					wnd.SetTitle("Gone!");
					break;
				case Mouse::Event::Type::Move:
					_oss << "Mouse Position : (" << _e.GetPosX() << ", " << _e.GetPosY() << ")";
					wnd.SetTitle(_oss.str());
					break;
				}
			}

		}

		if (gResult == -1)
		{
			return -1;
		}

		return msg.wParam;
	}
	catch (const DxException& _e)
	{
		MessageBox(nullptr, _e.what(), _e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& _e)
	{
		MessageBox(nullptr, _e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	return -1;
}