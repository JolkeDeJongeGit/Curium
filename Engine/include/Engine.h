#pragma once

class WinWindow;
namespace Engine
{
    void Init();
    void Update(const float inDt);
    void Shutdown();
    bool ShouldQuit();

    WinWindow* GetWindow();
}