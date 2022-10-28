#include "precomp.h"
#include "graphics/win32/WinHeapHandler.h"

void HeapHandler::CreateHeaps(uint32_t _bufferSize)
{
	m_constantBufferViewHeap = DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 65536, true);
	m_renderTargetViewHeap = DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, _bufferSize, false);
	m_depthStencilViewHeap = DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
}

DescriptorHeap& HeapHandler::GetRtvHeap()
{
	return m_renderTargetViewHeap;
}

DescriptorHeap& HeapHandler::GetCbvHeap()
{
	return m_constantBufferViewHeap;
}

DescriptorHeap& HeapHandler::GetDsvHeap()
{
	return m_depthStencilViewHeap;
}
