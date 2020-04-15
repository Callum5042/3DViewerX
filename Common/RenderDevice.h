#pragma once

#include "MainWindow.h"
#include <d3d11.h>

class RenderDevice
{
public:
	RenderDevice(MainWindow* window);

	bool Initialise();
	bool SetDepthStencil();
	void SetRasterState();

	void ClearScreen();
	void RenderScene();

	inline ID3D11Device* GetDevice() { return m_Device; }
	inline ID3D11DeviceContext* GetDeviceContext() { return m_DeviceContext; }

private:
	MainWindow* m_Window = nullptr;

	ID3D11Device* m_Device = nullptr;
	ID3D11DeviceContext* m_DeviceContext = nullptr;
	IDXGISwapChain* m_SwapChain = nullptr;
	ID3D11RenderTargetView* m_RenderTargetView = nullptr;
	ID3D11Texture2D* m_DepthStencil = nullptr;
	ID3D11DepthStencilView* m_DepthStencilView = nullptr;

	bool CreateDevice();
	bool CreateSwapChain();
	bool CreateRenderTargetView();
	void SetViewport();
};

namespace DX
{
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw std::exception();
		}
	}
}