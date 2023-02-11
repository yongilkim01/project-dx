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
	const float _t = timer.Peek();
	std::ostringstream _oss;
	_oss << "Time elapsed : " << std::setprecision(1) << std::fixed << _t << "s";
	wnd.SetTitle(_oss.str());
}