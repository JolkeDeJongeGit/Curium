#include "precomp.h"

#pragma warning( push )
#pragma warning(push, 0)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#pragma warning( pop ) 

#include "platform/win32/WinWindow.h"

WinWindow::~WinWindow()
{
	Shutdown();
}

void WinWindow::Create(int _width, int _height)
{
    SetWidth(_width);
    SetHeight(_height);

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    window = glfwCreateWindow(_width, _height, m_title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
}

void WinWindow::Update()
{
    SetActive(!glfwWindowShouldClose(window));

    glfwSwapBuffers(window);
    glfwPollEvents();
}

void WinWindow::Shutdown()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void WinWindow::SetIcon(int count, const char* name)
{
    if (windowIconImage)
    {
        delete windowIconImage;
        windowIconImage = nullptr;
    }

    windowIconImage = new GLFWimage();
    int width, height, channels;
    unsigned char* image_data = stbi_load(name, &width, &height, &channels, 0);

    // Create a GLFW image structure
    windowIconImage->width = width;
    windowIconImage->height = height;
    windowIconImage->pixels = image_data;
    glfwSetWindowIcon(window, count, windowIconImage);
    stbi_image_free(windowIconImage->pixels);
}
