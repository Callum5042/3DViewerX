#pragma once

#include "Renderer.h"
#include "Mesh.h"
#include <string>

class Sky
{
public:
	Sky(Renderer* renderer);

	void Load();
	void Render();

private:
	Renderer* m_Renderer = nullptr;

	Mesh* m_Mesh = nullptr;

	// Direct3D
	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11Buffer* m_IndexBuffer = nullptr;

	ID3D11ShaderResourceView* m_CubeMapSRV = nullptr;
	ID3D11Buffer* m_ConstantBuffer = nullptr;

	UINT m_IndexCount;

	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;

	void CreateVertexShader(std::string&& vertexShaderPath);
	void CreatePixelShader(std::string&& pixelShaderPath);

	ID3D11InputLayout* m_VertexLayout = nullptr;

	ID3D11DepthStencilState* DSLessEqual = nullptr;
};