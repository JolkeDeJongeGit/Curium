#pragma once
#include "GLFW3/include/GLFW/glfw3.h"
#include "platform/Window.h"

class WinWindow final : public Window
{
public:
    WinWindow() = default;
    virtual ~WinWindow();

    void Create(int inWidth, int inHeight) override;
    void Update() override;
    void Shutdown() override;

    void SetIcon(int inCount, const char* inName);

    inline GLFWwindow* GetWindow() const { return m_window; };

    static float MouseXOffset;
    static float MouseYOffset;
private:
#pragma warning( push )
#pragma warning( disable : 4100 )
    static void MouseCallback(GLFWwindow* inWindow, double inXPos, double inYPos);

#pragma warning( pop )
    GLFWwindow* m_window = nullptr;
    GLFWimage* m_windowIconImage = nullptr;
};