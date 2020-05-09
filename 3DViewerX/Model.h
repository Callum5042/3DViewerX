#pragma once

#include <d3d11_4.h>
#include <DirectXMath.h>
#include "Renderer.h"
#include <vector>
#include "WindowEvents.h"
#include <string>

struct SimpleVertex
{
	SimpleVertex(float x, float y, float z) : x(x), y(y), z(z) {}

	float x;
	float y;
	float z;

	float nx = 0;
	float ny = 0;
	float nz = 0;

	float u = 0;
	float v = 0;
};

class Model : public Events::WindowListener
{
public:
	Model(Renderer* renderer);

	bool Load(std::string&& filename);
	void Unload();

	void Update();
	void Render();

	constexpr bool IsLoaded() { return m_IsLoaded; };

private:
	DirectX::XMMATRIX m_World;

	ID3D11Buffer* m_ConstantBuffer = nullptr;

	float m_AxisX = 0.0f;
	float m_AxisY = 0.0f;
	float m_AxisZ = 0.0f;

	bool m_Wireframe = false;

	void SetRasterState();

	Renderer* m_Renderer = nullptr;

	std::vector<SimpleVertex> m_Vertices;
	std::vector<WORD> m_Indices;

	ID3D11ShaderResourceView* m_DiffuseMapSRV;

	bool m_IsLoaded = false;

	std::string m_Name;
};