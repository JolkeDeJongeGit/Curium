#pragma once
#include "GLFW3/include/GLFW/glfw3.h"
#include "platform/Window.h"

class WinWindow : public Window
{
public:
    WinWindow() = default;
    ~WinWindow();

    void Create(int _width, int _height) override;
    void Update() override;
    void Shutdown() override;

    void SetIcon(int count, const char* name);

    inline GLFWwindow* GetWindow() { return window; };

    static float MouseXOffset;
    static float MouseYOffset;
private:
#pragma warning( push )
#pragma warning( disable : 4100 )
    static void mouse_callback(GLFWwindow* window, double xPos, double yPos)
    {

        static bool firstMouse = true;
        static float lastX = 0.0f;
        static float lastY = 0.0f;
        if (firstMouse) {
            lastX = static_cast<float>(xPos);
            lastY = static_cast<float>(yPos);
            firstMouse = false;
        }

        MouseXOffset = static_cast<float>(xPos) - lastX;
        MouseYOffset = lastY - static_cast<float>(yPos);
        lastX = static_cast<float>(xPos);
        lastY = static_cast<float>(yPos);

    }
#pragma warning( pop )
    GLFWwindow* window = nullptr;
    GLFWimage* windowIconImage = nullptr;
};