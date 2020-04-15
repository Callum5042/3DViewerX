
#include "Shader.h"
#include <fstream>
#include <SDL_messagebox.h>

Shader::Shader(RenderDevice* renderDevice, MainWindow* window) : m_RenderDevice(renderDevice), m_Window(window)
{
}

bool Shader::CreateVertexShader(std::string&& vertexShaderPath)
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

	DX::ThrowIfFailed(m_RenderDevice->GetDevice()->CreateVertexShader(
		vertexbuffer,
		vertexsize,
		nullptr,
		&m_VertexShader));

	// Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

	ID3D11InputLayout* vertexLayout = nullptr;
	m_RenderDevice->GetDevice()->CreateInputLayout(layout, numElements, vertexbuffer, vertexsize, &vertexLayout);

	m_RenderDevice->GetDeviceContext()->IASetInputLayout(vertexLayout);

	delete[] vertexbuffer;
	return true;
}

bool Shader::CreatePixelShader(std::string&& pixelShaderPath)
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

	DX::ThrowIfFailed(m_RenderDevice->GetDevice()->CreatePixelShader(
		pixelbuffer,
		pixelsize,
		nullptr,
		&m_PixelShader));

	delete[] pixelbuffer;
	return true;
}

void Shader::BindShaders()
{
	m_RenderDevice->GetDeviceContext()->VSSetShader(m_VertexShader, nullptr, 0);
	m_RenderDevice->GetDeviceContext()->PSSetShader(m_PixelShader, nullptr, 0);
}
