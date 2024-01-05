#include "precomp.h"
#include "Engine.h"

#include <include/imgui_internal.h>

#include "graphics/Renderer.h"
#include "platform/win32/WinWindow.h"
#include "graphics/DebugManager.h"
#include "graphics/Camera.h"
#include <core/ImGuiLayer.h>
#include <core/Scene.h>

namespace Engine
{
	WinWindow* window;
	bool normal_mouse = false;
}

void Engine::Init()
{
	window = new WinWindow();
	window->SetTitle("Curium Engine");
	window->Create(1920, 1080);
	window->SetIcon(1, "assets/icon.png");

	Renderer::Init(window->GetWidth(), window->GetHeight());
	Scene::Init();
	Debug::Init();
	ImGuiLayer::Init();
}

void Engine::Update(const float inDt)
{
	PROFILE_FUNCTION()
	UpdateInput(inDt);

	ImGuiLayer::NewFrame();
	Renderer::Update();
	ImGuiLayer::UpdateWindow(inDt);

	if (!Debug::Paused())
	{
		Scene::Update(inDt);
	}

	Renderer::Render();
	window->Update();
}

void Engine::Shutdown()
{
	ImGuiLayer::Shutdown();
	Debug::Shutdown();
	Renderer::Shutdown();
}

void Engine::UpdateInput(const float inDt)
{
	//if(!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
	{
		if(glfwGetMouseButton(window->GetWindow(), GLFW_MOUSE_BUTTON_2))
		{
			if (glfwGetKey(window->GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
				Renderer::GetCamera()->ProcessKeyMovement(Direction::FORWARD, inDt);
			
			if (glfwGetKey(window->GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
				Renderer::GetCamera()->ProcessKeyMovement(Direction::BACKWARDS, inDt);
			
			if (glfwGetKey(window->GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
				Renderer::GetCamera()->ProcessKeyMovement(Direction::LEFT, inDt);
			
			if (glfwGetKey(window->GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
				Renderer::GetCamera()->ProcessKeyMovement(Direction::RIGHT, inDt);
			
			if (glfwGetKey(window->GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
				Renderer::GetCamera()->ProcessKeyMovement(Direction::UP, inDt);

			if (glfwGetKey(window->GetWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
				Renderer::GetCamera()->ProcessKeyMovement(Direction::DOWN, inDt);
			
			glfwSetInputMode(window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

			normal_mouse = false;
		}
		else if(!normal_mouse)
		{
			normal_mouse = true;
			glfwSetInputMode(window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

bool Engine::ShouldQuit()
{
	return !window->IsActive();
}

void Engine::Close()
{
	window->SetActive(false);
}

WinWindow* Engine::GetWindow()
{
	return Engine::window;
}
