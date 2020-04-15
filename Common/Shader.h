#pragma once

#include "RenderDevice.h"
#include <string>
#include <map>

class Shader
{
public:
	Shader(RenderDevice* renderDevice, MainWindow* window);

	bool CreateVertexShader(std::string&& vertexShaderPath);
	bool CreatePixelShader(std::string&& pixelShaderPath);

	void BindShaders();

private:
	RenderDevice* m_RenderDevice = nullptr;
	MainWindow* m_Window = nullptr;

	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;

	std::map<std::string, ID3D11Buffer*> m_BufferMap;
};