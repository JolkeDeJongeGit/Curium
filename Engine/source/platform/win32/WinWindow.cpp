#include "precomp.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <io.h>
#include <windowsx.h>
#include "platform/win32/WinWindow.h"

void WinWindow::Create(int _width, int _height)
{
	RECT wr;
	wr.left = 100;
	wr.right = _width + wr.left;
	wr.top = 100;
	wr.bottom = _height + wr.top;

	m_instance = GetModuleHandle(nullptr);

	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = EventHandler;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hCursor = nullptr;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();

	wc.hIcon = static_cast<HICON>(LoadImage(
		GetInstance(), MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON, 32, 32, 0
	));
		wc.hIconSm = static_cast<HICON>(LoadImage(
		GetInstance(), MAKEINTRESOURCE(IDI_ICON1),
		IMAGE_ICON, 16, 16, 0
	));

	RegisterClassEx(&wc);
	LPCWSTR value = (LPCWSTR)GetTitle().c_str();
	m_hwnd = CreateWindow(
		GetName(),
		value,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wr.right - wr.left,
		wr.bottom - wr.top,
		nullptr,        // We have no parent window.
		nullptr,        // We aren't using menus.
		m_instance,
		this);

	assert(m_hwnd && "Failed to create window");

	ShowWindow(m_hwnd, SW_NORMAL);

	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01; // mouse page
	rid.usUsage = 0x02; // mouse usage
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr;
	RegisterRawInputDevices(&rid, 1, sizeof(rid));
}

void WinWindow::Update()
{
	// Window Loop sends events.
	MSG msg = {};
	// Process any messages in the queue.
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void WinWindow::Terminate()
{
	UnregisterClass(GetName(), GetInstance());
}

void WinWindow::SetTitle(const std::wstring& _title)
{
	SetWindowText(m_hwnd, std::wstring(_title.begin(), _title.end()).c_str());
	Window::SetTitle(_title);
}

LRESULT WinWindow::EventHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		//SetActive(false);
		PostQuitMessage(0);
		return 0;
	}
	case WM_SIZE:
	{
		RECT clientRect = {};
		GetClientRect(hWnd, &clientRect);

		int width = clientRect.right - clientRect.left;
		int height = clientRect.bottom - clientRect.top;

		// Update width and height in the render

		return 0;
	}
	// Keyboard Input
	case WM_KEYDOWN: case WM_SYSKEYDOWN:
		break;
	case WM_KEYUP: case WM_SYSKEYUP:
		break;
		// Mouse Input
	case WM_MOUSEMOVE:
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_RBUTTONUP:
		break;
	}

	// Handle any messages the switch statement didn't.
	return DefWindowProc(hWnd, message, wParam, lParam);
}
