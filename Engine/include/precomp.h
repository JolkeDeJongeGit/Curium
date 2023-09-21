#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <memory>
#include <string>
#include <cassert>
#include <functional>
#include "common/Util.h"
#include "common/PerformanceManager.h"

#pragma region DirectX12
#include <wrl.h>
using namespace Microsoft::WRL;
#include <d3d12.h>
#include <dxgi1_6.h>
#include "directX12/d3dx12.h"
#pragma endregion

#include "resource.h"
