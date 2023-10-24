#include "precomp.h"
#include "graphics/win32/WinDevice.h"

Device::Device()
{
	SetupFactory();
	SetupDevice();
}

Device::~Device() = default;

const ComPtr<ID3D12Device2>& Device::GetDevice() const
{
	return m_device;
}

const ComPtr<IDXGIFactory4>& Device::GetFactory() const
{
	return m_factory;
}

void Device::SetupFactory()
{
	uint32_t dxgiFactoryFlags = 0;

#ifdef _DEBUG
	ID3D12Debug* dc;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&dc)));
//	dc->SetEnableAutoName(true);
	ThrowIfFailed(dc->QueryInterface(IID_PPV_ARGS(&m_debugController)));
	m_debugController->EnableDebugLayer();
	m_debugController->SetEnableGPUBasedValidation(true);

	dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

	dc->Release();
	dc = nullptr;
#endif

	HRESULT result = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory));
}

void Device::SetupDevice()
{
	ComPtr<IDXGIAdapter1> finalAdapter;
	ComPtr<IDXGIAdapter1> adapter1;
	SIZE_T maxVramSize = 0;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_factory->EnumAdapters1(i, &adapter1); i++)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter1->GetDesc1(&desc);

		// In case the GPU is a 'software' one instead of hardware
		// skip the adapter.
		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		if (SUCCEEDED(D3D12CreateDevice(adapter1.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), nullptr)))
		{
			if (desc.DedicatedVideoMemory > maxVramSize)
			{
				maxVramSize = desc.DedicatedVideoMemory;
				finalAdapter = adapter1;
			}
		}
	}

	ThrowIfFailed(D3D12CreateDevice(finalAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device)));
}
