#pragma once
#include "graphics/win32/WinDescriptorHeap.h"

class HeapHandler
{
public:
	static HeapHandler& Get()
	{
		static HeapHandler instance;
		return instance;
	}

	void CreateHeaps(uint32_t inBufferSize);
	DescriptorHeap& GetRtvHeap();
	DescriptorHeap& GetCbvHeap();
	DescriptorHeap& GetDsvHeap();
private:
	DescriptorHeap m_renderTargetViewHeap;
	DescriptorHeap m_constantBufferViewHeap;
	DescriptorHeap m_depthStencilViewHeap;
};