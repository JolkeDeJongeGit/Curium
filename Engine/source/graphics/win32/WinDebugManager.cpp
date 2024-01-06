#include "precomp.h"
#include "graphics/DebugManager.h"

namespace Debug
{
    bool paused = false;
    bool show_profiler = false;
    bool wireframe_mode = false;
    int selected_game_object;
}

void Debug::Init()
{

}

void Debug::Update(const float inDt)
{
}

bool& Debug::Paused()
{
    return paused;
}

bool& Debug::IsWireframeMode()
{
    return wireframe_mode;
}

void Debug::Shutdown()
{

}