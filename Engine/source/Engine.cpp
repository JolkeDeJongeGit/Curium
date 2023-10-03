#include "precomp.h"
#include "Engine.h"
#include "graphics/Renderer.h"
#include "platform/win32/WinWindow.h"
#include "graphics/DebugManager.h"
#include "graphics/Camera.h"

namespace Engine
{
	WinWindow* window;
}

void Engine::Init()
{
	window = new WinWindow();
	window->SetTitle("Curium Engine");
	window->Create(1920, 1080);
	window->SetIcon(1, "assets/icon.png");

	Renderer::Init(window->GetWidth(), window->GetHeight());
	Debug::Init();
}

void Engine::Update(const float inDt)
{
	PROFILE_FUNCTION()
	UpdateInput(inDt);
	Debug::Update(inDt);
	Renderer::Update();
	if (!Debug::Paused())
	{
		
	}

	Renderer::Render();
	window->Update();
}

void Engine::Shutdown()
{
	Debug::Shutdown();
	Renderer::Shutdown();
}

void Engine::UpdateInput(const float inDt)
{
	if (glfwGetKey(window->GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
		Renderer::GetCamera()->ProcessKeyboard(FORWARD, inDt);

	if (glfwGetKey(window->GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
		Renderer::GetCamera()->ProcessKeyboard(BACKWARD, inDt);

	if (glfwGetKey(window->GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
		Renderer::GetCamera()->ProcessKeyboard(LEFT, inDt);

	if (glfwGetKey(window->GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
		Renderer::GetCamera()->ProcessKeyboard(RIGHT, inDt);

}

bool Engine::ShouldQuit()
{
	return !window->IsActive();
}

WinWindow* Engine::GetWindow()
{
	return Engine::window;
}
