#pragma once

#include <d3d11_4.h>
#include <string>
#include "MainWindow.h"

class Renderer
{
public:
	Renderer(MainWindow* mainWindow);

	bool Init();
	void ClearScreen();
	void Render();
	void Resize(int width, int height);

	ID3D11Device* GetDevice() const { return m_Device; }
	ID3D11DeviceContext* GetDeviceContext() const { return m_DeviceContext; }

	ID3D11ShaderResourceView* GetTextureMap() { return m_TextureMap; }

	ID3D11RenderTargetView* m_RenderTargetView = nullptr;
	ID3D11RenderTargetView* m_TextureRenderTargetView = nullptr;

	ID3D11DepthStencilView* m_DepthStencilView = nullptr;
	ID3D11DepthStencilView* m_TextureDepthStencilView = nullptr;

private:
	ID3D11Device* m_Device = nullptr;
	ID3D11DeviceContext* m_DeviceContext = nullptr;
	IDXGISwapChain* m_SwapChain = nullptr;
	ID3D11Texture2D* m_DepthStencil = nullptr;
	ID3D11Texture2D* m_TextureDepthStencil = nullptr;
	ID3D11Buffer* m_ConstantBuffer = nullptr;

	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;

	bool CreateDevice();
	bool CreateSwapChain();
	bool CreateRenderTargetView();
	void SetViewport();

	UINT m_4xMsaaQuality;

	bool CreateVertexShader(std::string&& vertexShaderPath);
	bool CreatePixelShader(std::string&& pixelShaderPath);

	IDXGIFactory1* GetDXGIFactory();
	HWND GetHwnd() const;

	MainWindow* m_MainWindow = nullptr;

	ID3D11Texture2D* m_Texture = nullptr;
	ID3D11ShaderResourceView* m_TextureMap = nullptr;
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