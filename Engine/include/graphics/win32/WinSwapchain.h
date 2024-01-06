#pragma once
class Swapchain
{
public:
	Swapchain();
	~Swapchain();

	void Init(int inWidth, int inHeight);
	
	uint32_t GetCurrentBuffer() const;

	ComPtr<IDXGISwapChain4>& GetSwapchain();

	void ResizeBuffer(int _width, int _height) const;

	void Present();

	void UpdateFenceValue(); 
	void WaitForFenceValue(const ComPtr<ID3D12CommandQueue>& inCommandQueue) const;

	inline ComPtr<ID3D12Fence>& GetFence() { return m_fence; };
	inline ComPtr<ID3D12Resource>& GetCurrentRenderTarget(const uint32_t inIndex) { return m_renderTargets[inIndex]; };
	inline ComPtr<ID3D12Resource>& GetDepthBuffer() { return m_depthBuffer; };
	inline ComPtr<ID3D12Resource>& GetRenderTextureBuffer() { return m_renderTextureBuffer; };
	static constexpr uint32_t BackBufferCount = 3;

	int m_renderTextureSrvID;
	uint32_t m_renderTextureHeapID;
private:
	void SetupSwapchain(int inWidth, int inHeight);
	void SetupDepthBuffer(int inWidth, int inHeight);
	void SetupRenderTextureBuffer(int inWidth, int inHeight);

	uint32_t m_currentBuffer = 0;
	 
	ComPtr<ID3D12Resource> m_renderTargets[BackBufferCount] {};
	ComPtr<ID3D12Resource> m_renderTextureBuffer = nullptr;
	ComPtr<ID3D12Resource> m_depthBuffer = nullptr;

	ComPtr<ID3D12Fence> m_fence = nullptr;

	ComPtr<IDXGISwapChain4> m_swapchain = nullptr;

	HANDLE m_fenceEvent = nullptr;
	uint64_t m_fenceValue = 0;

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_surfaceSize;
};