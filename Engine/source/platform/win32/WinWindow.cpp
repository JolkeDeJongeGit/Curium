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

void WinWindow::Create(const int inWidth, const int inHeight)
{
    SetWidth(inWidth);
    SetHeight(inHeight);

    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    m_window = glfwCreateWindow(inWidth, inHeight, m_title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(m_window);
    glfwSetCursorPosCallback(m_window, MouseCallback);
}

void WinWindow::Update()
{
    SetActive(!glfwWindowShouldClose(m_window));

    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void WinWindow::Shutdown()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void WinWindow::SetIcon(const int inCount, const char* inName)
{
    if (m_windowIconImage)
    {
        delete m_windowIconImage;
        m_windowIconImage = nullptr;
    }

    m_windowIconImage = new GLFWimage();
    int width, height, channels;
    unsigned char* imageData = stbi_load(inName, &width, &height, &channels, 0);

    // Create a GLFW image structure
    m_windowIconImage->width = width;
    m_windowIconImage->height = height;
    m_windowIconImage->pixels = imageData;
    glfwSetWindowIcon(m_window, inCount, m_windowIconImage);
    stbi_image_free(m_windowIconImage->pixels);
}
