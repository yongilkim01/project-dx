#include "App.h"

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow) 
{
	try
	{
		return App{}.Go();
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