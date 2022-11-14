#include "pch.h"
#include <platform/win32/WinWindow.h>
#include <graphics/Renderer.h>

int main()
{
	auto& win = WinWindow::Get();
	win.Create(1920, 1080);
	win.SetTitle(L"Curium");

	Renderer ren;
	ren.Init(1920, 1080);
	while (win.IsActive())
	{
		win.Update();
		ren.Render();
	}

	return 0;
}