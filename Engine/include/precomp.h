#pragma once
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/transform.hpp"
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

#include "graphics/win32/WinUtil.h"
#include "resource.h"
