#pragma once

#include <d3d11_4.h>
#include <DirectXMath.h>
#include "Renderer.h"
using namespace DirectX;

class Model
{
public:
	Model(Renderer* renderer);

	bool Load();

	void Update();
	void Render();

private:
	XMMATRIX m_World;
	XMMATRIX m_View;
	XMMATRIX m_Projection;

	ID3D11Buffer* m_ConstantBuffer = nullptr;

	float m_AxisX = 0.0f;
	float m_AxisY = 0.0f;
	float m_AxisZ = 0.0f;

	bool m_Wireframe = false;

	void SetRasterState();

	Renderer* m_Renderer = nullptr;
};