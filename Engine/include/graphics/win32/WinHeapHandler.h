#pragma once
#include "graphics/win32/WinDescriptorHeap.h"

class HeapHandler
{
public:
	~HeapHandler();
	void CreateHeaps(uint32_t inBufferSize);
	[[nodiscard]] DescriptorHeap* GetRtvHeap() const;
	DescriptorHeap* GetCbvHeap() const;
	DescriptorHeap* GetDsvHeap() const;
private:
	DescriptorHeap* m_renderTargetViewHeap = nullptr;
	DescriptorHeap* m_constantBufferViewHeap = nullptr;
	DescriptorHeap* m_depthStencilViewHeap = nullptr;
};