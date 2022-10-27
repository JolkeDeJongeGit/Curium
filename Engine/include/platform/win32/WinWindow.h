#pragma once
#include "CustomWin.h"
#include "platform/Window.h"

class WinWindow : public Window
{
public:
	void Create(int _width, int _height) override;
	void Update() override;
	void Terminate() override;
	void SetTitle(const std::wstring& _title) override;

	const wchar_t* GetName() { return m_className.c_str();  }

	HWND GetHwnd() { return m_hwnd; }
	HINSTANCE GetInstance() { return m_instance; }

protected:
	static LRESULT CALLBACK EventHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
	HWND m_hwnd;
	HINSTANCE m_instance;

	std::wstring m_className = L"WinClass";
};