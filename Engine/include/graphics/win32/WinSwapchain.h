#pragma once
class Swapchain
{
public:
	Swapchain();
	~Swapchain();

	void Init(int _width, int _height);

	static Swapchain& Get()
	{
		static Swapchain instance;
		return instance;
	}

	const uint32_t GetCurrentBuffer();

	ComPtr<IDXGISwapChain4>& GetSwapchain();

	void ResizeBuffer(int _width, int _height);

	void Present();

	void UpdateFenceValue(); 
	void WaitForFenceValue(ComPtr<ID3D12CommandQueue>& _commandQueue);

	ComPtr<ID3D12Fence>& GetFence();
	ComPtr<ID3D12Resource>& GetCurrentRenderTarget(const uint32_t _index);
	ComPtr<ID3D12Resource>& GetDepthBuffer();
	static const uint32_t BackBufferCount = 2;
private:
	void SetupSwapchain(int _width, int _height);
	void SetupDepthBuffer(int _width, int _height);

	uint32_t m_currentBuffer = 0;
	 
	ComPtr<ID3D12Resource> m_renderTargets[BackBufferCount] {};
	ComPtr<ID3D12Resource> m_depthBuffer = nullptr;

	ComPtr<ID3D12Fence> m_fence = nullptr;

	ComPtr<IDXGISwapChain4> m_swapchain = nullptr;

	HANDLE m_fenceEvent = nullptr;
	uint64_t m_fenceValue = 0;

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_surfaceSize;
};