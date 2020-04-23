#pragma once

#include <d3d11_4.h>
#include <DirectXMath.h>
using namespace DirectX;

class Model
{
public:

	bool Load(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	void Render(ID3D11DeviceContext* deviceContext);

private:
	XMMATRIX m_World;
	XMMATRIX m_View;
	XMMATRIX m_Projection;

	ID3D11Buffer* m_ConstantBuffer = nullptr;
};