#include "Renderer.h"
#include <SDL_syswm.h>
#include <fstream>
#include <SDL_messagebox.h>
#include <DirectXMath.h>

struct LightBuffer
{
	DirectX::XMFLOAT4 mDirection;
};

Renderer::Renderer(MainWindow* mainWindow) : m_MainWindow(mainWindow)
{
}

bool Renderer::Init()
{
	if (!CreateDevice())
		return false;

	if (!CreateSwapChain())
		return false;

	if (!CreateRenderTargetView())
		return false;

	if (!CreateTextureRenderTargetView(800, 600))
		return false;

	SetViewport();

	// Create shaders
	if (!CreateVertexShader("D:\\Sources\\3DViewerX\\bin\\3DViewerX\\Debug-x64\\VertexShader.cso"))
		return false;

	if (!CreatePixelShader("D:\\Sources\\3DViewerX\\bin\\3DViewerX\\Debug-x64\\PixelShader.cso"))
		return false;

	// Light buffer
	D3D11_BUFFER_DESC bd1 = {};
	bd1.Usage = D3D11_USAGE_DEFAULT;
	bd1.ByteWidth = sizeof(LightBuffer);
	bd1.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd1.CPUAccessFlags = 0;
	HRESULT hr = GetDevice()->CreateBuffer(&bd1, nullptr, &m_LightBuffer);
	if (FAILED(hr))
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "CreateBuffer failed", nullptr);
		return false;
	}

	return true;
}

void Renderer::ClearScreen()
{
	// Clear to blue
	static const float DarkBlue[] = { 0.0f, 0.0f, 0.2f, 1.0f };
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, reinterpret_cast<const float*>(&DarkBlue));
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	static const float Blue[] = { 0.3f, 0.5f, 0.7f, 1.0f };
	m_DeviceContext->ClearRenderTargetView(m_TextureRenderTargetView, reinterpret_cast<const float*>(&Blue));
	m_DeviceContext->ClearDepthStencilView(m_TextureDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Bind shaders
	m_DeviceContext->VSSetShader(m_VertexShader, nullptr, 0);
	m_DeviceContext->PSSetShader(m_PixelShader, nullptr, 0);

	// Light ting
	LightBuffer lb;
	lb.mDirection = DirectX::XMFLOAT4(m_LightDirX, m_LightDirY, m_LightDirZ, 0.0f);

	GetDeviceContext()->VSSetConstantBuffers(1, 1, &m_LightBuffer);
	GetDeviceContext()->PSSetConstantBuffers(1, 1, &m_LightBuffer);
	GetDeviceContext()->UpdateSubresource(m_LightBuffer, 0, nullptr, &lb, 0, 0);
}

void Renderer::Render()
{
	m_SwapChain->Present(0, 0);
}

void Renderer::Resize(int width, int height)
{
	m_RenderTargetView->Release();
	m_DepthStencilView->Release();

	DX::ThrowIfFailed(m_SwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	CreateRenderTargetView();
	SetViewport();
}

void Renderer::ResizeViewport(int width, int height)
{
	m_TextureRenderTargetView->Release();
	m_TextureDepthStencilView->Release();
	CreateTextureRenderTargetView(width, height);
}

void Renderer::SetViewportTarget(int width, int height)
{
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<FLOAT>(width);
	vp.Height = static_cast<FLOAT>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	m_DeviceContext->RSSetViewports(1, &vp);
	m_DeviceContext->OMSetRenderTargets(1, &m_TextureRenderTargetView, m_TextureDepthStencilView);
}

void Renderer::SetWindowTarget()
{
	SetViewport();
	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);
}

bool Renderer::CreateDevice()
{
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	D3D_FEATURE_LEVEL featureLevel;
	DX::ThrowIfFailed(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, featureLevels, numFeatureLevels, D3D11_SDK_VERSION, &m_Device, &featureLevel, &m_DeviceContext));

	if (featureLevel != D3D_FEATURE_LEVEL_11_1)
	{
		return false;
	}

	DX::ThrowIfFailed(m_Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4xMsaaQuality));
	return true;
}

bool Renderer::CreateSwapChain()
{
	IDXGIFactory1* dxgiFactory1 = GetDXGIFactory();
	IDXGIFactory2* dxgiFactory2 = nullptr;
	DX::ThrowIfFailed(dxgiFactory1->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2)));
	if (dxgiFactory2 != nullptr)
	{
		ID3D11Device1* device1 = nullptr;
		DX::ThrowIfFailed(m_Device->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&device1)));

		ID3D11DeviceContext1* deviceContext1 = nullptr;
		(void)m_DeviceContext->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&deviceContext1));

		DXGI_SWAP_CHAIN_DESC1 sd = {};
		sd.Width = m_MainWindow->GetWidth();
		sd.Height = m_MainWindow->GetHeight();
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m_4xMsaaQuality - 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;

		IDXGISwapChain1* swapChain1 = nullptr;
		DX::ThrowIfFailed(dxgiFactory2->CreateSwapChainForHwnd(m_Device, GetHwnd(), &sd, nullptr, nullptr, &swapChain1));

		DX::ThrowIfFailed(swapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&m_SwapChain)));

		dxgiFactory2->Release();
	}
	else
	{
		DXGI_SWAP_CHAIN_DESC sd = {};
		sd.BufferCount = 1;
		sd.BufferDesc.Width = m_MainWindow->GetWidth();
		sd.BufferDesc.Height = m_MainWindow->GetHeight();
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = GetHwnd();
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m_4xMsaaQuality - 1;
		sd.Windowed = TRUE;

		DX::ThrowIfFailed(dxgiFactory1->CreateSwapChain(m_Device, &sd, &m_SwapChain));
	}

	dxgiFactory1->MakeWindowAssociation(GetHwnd(), DXGI_MWA_NO_ALT_ENTER);

	dxgiFactory1->Release();
	return true;
}

bool Renderer::CreateRenderTargetView()
{
	// Render target view
	ID3D11Resource* backBuffer = nullptr;
	DX::ThrowIfFailed(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&backBuffer)));
	if (backBuffer == nullptr)
	{
		return false;
	}

	DX::ThrowIfFailed(m_Device->CreateRenderTargetView(backBuffer, nullptr, &m_RenderTargetView));
	backBuffer->Release();

	// Depth stencil view
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = m_MainWindow->GetWidth();
	descDepth.Height = m_MainWindow->GetHeight();
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 4;
	descDepth.SampleDesc.Quality = m_4xMsaaQuality - 1;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	DX::ThrowIfFailed(m_Device->CreateTexture2D(&descDepth, nullptr, &m_DepthStencil));
	if (m_DepthStencil == nullptr)
	{
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	descDSV.Texture2D.MipSlice = 0;
	DX::ThrowIfFailed(m_Device->CreateDepthStencilView(m_DepthStencil, &descDSV, &m_DepthStencilView));

	return true;
}

bool Renderer::CreateTextureRenderTargetView(int width, int height)
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 4;
	texDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	DX::ThrowIfFailed(m_Device->CreateTexture2D(&texDesc, 0, &m_Texture));

	// Create the render target view.
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = texDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	DX::ThrowIfFailed(m_Device->CreateRenderTargetView(m_Texture, &renderTargetViewDesc, &m_TextureRenderTargetView));

	// Depth stencil view
	D3D11_TEXTURE2D_DESC texDepthDesc;
	texDepthDesc.Width = width;
	texDepthDesc.Height = height;
	texDepthDesc.MipLevels = 1;
	texDepthDesc.ArraySize = 1;
	texDepthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texDepthDesc.SampleDesc.Count = 4;
	texDepthDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
	texDepthDesc.Usage = D3D11_USAGE_DEFAULT;
	texDepthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	texDepthDesc.CPUAccessFlags = 0;
	texDepthDesc.MiscFlags = 0;
	DX::ThrowIfFailed(m_Device->CreateTexture2D(&texDepthDesc, nullptr, &m_TextureDepthStencil));

	DX::ThrowIfFailed(m_Device->CreateDepthStencilView(m_TextureDepthStencil, nullptr, &m_TextureDepthStencilView));

	// Create the shader resource view.
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	shaderResourceViewDesc.Format = texDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	DX::ThrowIfFailed(m_Device->CreateShaderResourceView(m_Texture, &shaderResourceViewDesc, &m_TextureMap));

	return true;
}

void Renderer::SetViewport()
{
	D3D11_VIEWPORT vp;
	vp.Width = static_cast<FLOAT>(m_MainWindow->GetWidth());
	vp.Height = static_cast<FLOAT>(m_MainWindow->GetHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	m_DeviceContext->RSSetViewports(1, &vp);
}

IDXGIFactory1* Renderer::GetDXGIFactory()
{
	IDXGIFactory1* dxgiFactory = nullptr;
	{
		IDXGIDevice* dxgiDevice = nullptr;
		DX::ThrowIfFailed(m_Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice)));

		IDXGIAdapter* adapter = nullptr;
		DX::ThrowIfFailed(dxgiDevice->GetAdapter(&adapter));

		DX::ThrowIfFailed(adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory)));

		adapter->Release();
		dxgiDevice->Release();
	}

	return dxgiFactory;
}

HWND Renderer::GetHwnd() const
{
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);

	SDL_Window* window = reinterpret_cast<SDL_Window*>(m_MainWindow->GetWindow());
	SDL_GetWindowWMInfo(window, &wmInfo);
	return wmInfo.info.win.window;
}

bool Renderer::CreateVertexShader(std::string&& vertexShaderPath)
{
	std::ifstream vertexFile(vertexShaderPath, std::fstream::in | std::fstream::binary);
	if (!vertexFile.is_open())
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not read VertexShader.cso", nullptr);
		return false;
	}

	vertexFile.seekg(0, vertexFile.end);
	int vertexsize = (int)vertexFile.tellg();
	vertexFile.seekg(0, vertexFile.beg);

	char* vertexbuffer = new char[vertexsize];
	vertexFile.read(vertexbuffer, vertexsize);

	DX::ThrowIfFailed(m_Device->CreateVertexShader(vertexbuffer, vertexsize, nullptr, &m_VertexShader));

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT numElements = ARRAYSIZE(layout);

	ID3D11InputLayout* vertexLayout = nullptr;
	m_Device->CreateInputLayout(layout, numElements, vertexbuffer, vertexsize, &vertexLayout);
	m_DeviceContext->IASetInputLayout(vertexLayout);

	delete[] vertexbuffer;
	return true;
}

bool Renderer::CreatePixelShader(std::string&& pixelShaderPath)
{
	std::ifstream pixelFile(pixelShaderPath, std::fstream::in | std::fstream::binary);
	if (!pixelFile.is_open())
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not read PixelShader.cso", nullptr);
		return false;
	}

	pixelFile.seekg(0, pixelFile.end);
	int pixelsize = (int)pixelFile.tellg();
	pixelFile.seekg(0, pixelFile.beg);

	char* pixelbuffer = new char[pixelsize];
	pixelFile.read(pixelbuffer, pixelsize);

	DX::ThrowIfFailed(m_Device->CreatePixelShader(pixelbuffer, pixelsize, nullptr, &m_PixelShader));

	delete[] pixelbuffer;
	return true;
}