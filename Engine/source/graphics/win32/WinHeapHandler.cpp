#include "precomp.h"
#include "graphics/win32/WinHeapHandler.h"

HeapHandler::~HeapHandler()
{
	delete m_depthStencilViewHeap;
	delete m_constantBufferViewHeap;
	delete m_renderTargetViewHeap;
	
	m_renderTargetViewHeap = nullptr;
	m_constantBufferViewHeap = nullptr;
	m_depthStencilViewHeap = nullptr;
}

void HeapHandler::CreateHeaps(const uint32_t inBufferSize)
{
	m_constantBufferViewHeap = new DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 65536, true);
	m_renderTargetViewHeap = new DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, inBufferSize, false);
	m_depthStencilViewHeap = new DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
}

DescriptorHeap* HeapHandler::GetRtvHeap() const
{
	return m_renderTargetViewHeap;
}

DescriptorHeap* HeapHandler::GetCbvHeap() const
{
	return m_constantBufferViewHeap;
}

DescriptorHeap* HeapHandler::GetDsvHeap() const
{
	return m_depthStencilViewHeap;
}
