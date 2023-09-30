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
    static void MouseCallback(GLFWwindow* inWindow, double inXPos, double inYPos)
    {
        static bool firstMouse = true;
        static float lastX = 0.0f;
        static float lastY = 0.0f;
        if (firstMouse) {
            lastX = static_cast<float>(inXPos);
            lastY = static_cast<float>(inYPos);
            firstMouse = false;
        }

        MouseXOffset = static_cast<float>(inXPos) - lastX;
        MouseYOffset = lastY - static_cast<float>(inYPos);
        lastX = static_cast<float>(inXPos);
        lastY = static_cast<float>(inYPos);

    }
#pragma warning( pop )
    GLFWwindow* m_window = nullptr;
    GLFWimage* m_windowIconImage = nullptr;
};