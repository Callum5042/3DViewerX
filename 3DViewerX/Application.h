#pragma once

#include "Engine.h"
#include "WindowEvents.h"

#include <d3d11_4.h>

class Application : public Engine, public Events::WindowListener
{
public:
	Application() = default;

	bool OnInitialise() override;
	void OnUpdate() override;
	void OnRender() override;

	// Window Events
	void OnQuit() override;
	void OnResize(int width, int height) override;

private:

	ID3D11Device* m_Device = nullptr;
	ID3D11DeviceContext* m_DeviceContext = nullptr;
	IDXGISwapChain* m_SwapChain = nullptr;
	ID3D11Texture2D* m_DepthStencil = nullptr;
	ID3D11RenderTargetView* m_RenderTargetView = nullptr;
	ID3D11DepthStencilView* m_DepthStencilView = nullptr;

	UINT m_4xMsaaQuality;

	bool CreateDevice();
	bool CreateSwapChain();
	bool CreateRenderTargetView();
	void SetViewport();

	IDXGIFactory1* GetDXGIFactory();
	HWND GetHwnd() const;
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