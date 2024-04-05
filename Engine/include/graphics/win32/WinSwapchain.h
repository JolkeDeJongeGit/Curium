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

	inline ComPtr<ID3D12Resource>& GetCurrentRenderTarget(const uint32_t inIndex) { return m_renderTargets[inIndex]; };
	inline ComPtr<ID3D12Resource>& GetDepthBuffer() { return m_depthBuffer; };
	inline ComPtr<ID3D12Resource>& GetRenderTextureBuffer() { return m_renderTextureBuffer; };
	static constexpr uint32_t BackBufferCount = 3;

	uint32_t m_renderTextureSrvID;
	uint32_t m_renderTextureHeapID;
	uint32_t m_depthTextureSrvID;
private:
	void SetupSwapchain(int inWidth, int inHeight);
	void SetupDepthBuffer(int inWidth, int inHeight);
	void SetupRenderTextureBuffer(int inWidth, int inHeight);

	uint32_t m_currentBuffer = 0;
	 
	ComPtr<ID3D12Resource> m_renderTargets[BackBufferCount] {};
	ComPtr<ID3D12Resource> m_renderTextureBuffer = nullptr;
	ComPtr<ID3D12Resource> m_depthBuffer = nullptr;

	uint64_t m_fenceValues[BackBufferCount]{0};

	ComPtr<IDXGISwapChain4> m_swapchain = nullptr;

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_surfaceSize;
};