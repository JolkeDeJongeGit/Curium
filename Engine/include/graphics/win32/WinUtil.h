#pragma once

class Swapchain;
class Device;
class CommandQueue;
class WinWindow;
class DescriptorHeap;
class PipelineState;
class ShaderManager;

/// <summary>
/// namespace with functions that access the user to
/// Custom DX components like DXDevice.
/// </summary>
enum class HeapType
{
	CBV_SRV_UAV, // Constant buffer/Shader resource/Unordered access views
	DSV,		 // Depth stencil view
	RTV			 // Render target view
};

#define NAMED_RESOURCE(obj, name) (obj)->SetName(L#name)

/// <summary>
/// Global getters that are defined in the Windows implementation of renderer
/// </summary>
namespace WinUtil
{
	Device* GetDevice();
	Swapchain* GetSwapchain();
	CommandQueue* GetCommandQueue();
	WinWindow* GetWindow();
	ShaderManager* GetShaderManager();
	DescriptorHeap* GetDescriptorHeap(HeapType inType);
}