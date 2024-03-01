#include "precomp.h"
#include "Engine.h"

#include <include/imgui_internal.h>

#include "graphics/Renderer.h"
#include "platform/win32/WinWindow.h"
#include "graphics/DebugManager.h"
#include "graphics/Camera.h"
#include <core/ImGuiLayer.h>
#include <core/Scene.h>
#include <core/Editor.h>

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

	Renderer::Update();

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
	Editor::Shutdown();
	Renderer::Shutdown();
}

void Engine::UpdateInput(const float inDt)
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;



	if(ImGui::IsMouseDown(1))
	{
		//if (Editor::ViewportHovered())
		{
			if (ImGui::IsKeyDown(ImGuiKey_W))
				Renderer::GetCamera()->ProcessKeyMovement(Direction::FORWARD, inDt);

			if (ImGui::IsKeyDown(ImGuiKey_S))
				Renderer::GetCamera()->ProcessKeyMovement(Direction::BACKWARDS, inDt);

			if (ImGui::IsKeyDown(ImGuiKey_A))
				Renderer::GetCamera()->ProcessKeyMovement(Direction::LEFT, inDt);

			if (ImGui::IsKeyDown(ImGuiKey_D))
				Renderer::GetCamera()->ProcessKeyMovement(Direction::RIGHT, inDt);

			if (ImGui::IsKeyDown(ImGuiKey_Space))
				Renderer::GetCamera()->ProcessKeyMovement(Direction::UP, inDt);

			if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
				Renderer::GetCamera()->ProcessKeyMovement(Direction::DOWN, inDt);

			

			glfwSetInputMode(window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			ImGui::SetMouseCursor(ImGuiMouseCursor_None);
			normal_mouse = false;

		}
	}
	else if(!normal_mouse)
	{
		normal_mouse = true;
		glfwSetInputMode(window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
