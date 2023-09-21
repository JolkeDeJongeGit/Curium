#include "precomp.h"
#include "Engine.h"
#include "graphics/Renderer.h"
#include "platform/win32/WinWindow.h"
#include "graphics/DebugManager.h"

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

void Engine::Update(const float dt)
{
	PROFILE_FUNCTION();
	Debug::Update(dt);
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

bool Engine::ShouldQuit()
{
	return !window->IsActive();
}

WinWindow* Engine::GetWindow()
{
	return Engine::window;
}
