#pragma once
#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d12.h>
#include <dxgi1_6.h>

class Device
{
public:
	Device();
	~Device();


	static Device& Get()
	{
		static Device m_instance;
		return m_instance;
	}

	ComPtr<ID3D12Device2>& GetDevice();
	ComPtr<IDXGIFactory4>& GetFactory();
private:
	void SetupFactory();
	void SetupDevice();

	ComPtr<ID3D12Device2> m_device;
	ComPtr<IDXGIFactory4> m_factory;

#ifdef _DEBUG
	ComPtr<ID3D12Debug1> m_debugController;
#endif

};