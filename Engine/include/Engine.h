#pragma once

class WinWindow;
namespace Engine
{
    void Init();
    void Update(const float dt);
    void Shutdown();
    bool ShouldQuit();

    WinWindow* GetWindow();
}