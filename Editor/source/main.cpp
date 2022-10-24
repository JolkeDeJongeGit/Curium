#include "pch.h"
#include <platform/win32/WinWindow.h>

int main()
{
	WinWindow win;
	win.Create(1920, 1080);
	win.SetTitle(L"Curium");

	while (win.IsActive())
	{
		win.Update();
	}

	return 0;
}