#pragma once

class WinWindow;
namespace Engine
{
    void Init();
    void Update(const float inDt);
    void Shutdown();
    void UpdateInput(const float inDt);
    bool ShouldQuit();
    void Close();
    WinWindow* GetWindow();
}