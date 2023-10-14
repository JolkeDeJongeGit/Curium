#include "pch.h"
#include <Engine.h>

int main()
{
    float deltaTime = 0.1f;
    Engine::Init();
    std::chrono::steady_clock::time_point previousTime = std::chrono::high_resolution_clock::now();
    while (!Engine::ShouldQuit())
    {
        std::chrono::steady_clock::time_point currentTime;
        currentTime = std::chrono::high_resolution_clock::now();
        Engine::Update(deltaTime);
        deltaTime = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(
            currentTime - previousTime).count()) * 0.001f;
        previousTime = currentTime;
    }
    Engine::Shutdown();
	return 0;
}