#include "App.h"

App::App()
	:
	wnd(800, 600, "Direct3D Game")
{}

int App::Go()
{
	while (true)
	{
		if (const auto _ecode = Window::ProcessMessages())
		{
			return *_ecode;
		}
		DoFrame();
	}
}

void App::DoFrame()
{
	const float c = sin(timer.Peek()) / 2.0f + 0.5f;
	wnd.Graphic().ClearBuffer(c, c, 1.0f);
	wnd.Graphic().EndFrame();
}