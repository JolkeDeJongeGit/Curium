#include "pch.h"
#include <Engine.h>

int main()
{
    Engine::Init();
    std::chrono::steady_clock::time_point previousTime = std::chrono::high_resolution_clock::now();
    while (!Engine::ShouldQuit())
    {
        std::chrono::steady_clock::time_point currentTime;
        currentTime = std::chrono::high_resolution_clock::now();
        const float deltaTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(
            currentTime - previousTime).count()) * 0.001f;
        previousTime = currentTime;
        Engine::Update(deltaTime);
    }
    Engine::Shutdown();
	return 0;
}