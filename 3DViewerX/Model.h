#pragma once

#include <d3d11_4.h>
#include <DirectXMath.h>
#include "Renderer.h"
#include <vector>
#include "WindowEvents.h"
#include "InputEvents.h"
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

	float tx = 0;
	float ty = 0;
	float tz = 0;

	float bx = 0;
	float by = 0;
	float bz = 0;
};

struct Mesh
{
	std::vector<SimpleVertex> vertices;
	std::vector<WORD> indices;

	int startIndex = 0;
	int startVertex = 0;
	std::string name;
};

class Model : public Events::WindowListener, public Events::InputListener
{
public:
	Model(Renderer* renderer);

	bool Load(std::string&& filename);
	void Unload();

	void Gui();
	void Update();
	void Render();

	constexpr bool IsLoaded() { return m_IsLoaded; };

	// Mouse events
	void OnMouseMotion(MouseData&& data) override;

private:
	Renderer* m_Renderer = nullptr;

	// Direct3D
	DirectX::XMMATRIX m_World;
	ID3D11Buffer* m_ConstantBuffer = nullptr;
	ID3D11ShaderResourceView* m_DiffuseMapSRV = nullptr;
	ID3D11ShaderResourceView* m_NormalMapSRV = nullptr;

	// Rotation
	float m_Pitch = 0.0f;
	float m_Yaw = 0.0f;
	float m_Roll = 0.0f;

	// Scale
	float m_ScaleX = 1.0f;
	float m_ScaleY = 1.0f;
	float m_ScaleZ = 1.0f;

	// Translation
	float m_PositionX = 0.0f;
	float m_PositionY = 0.0f;
	float m_PositionZ = 0.0f;

	// Wireframe
	bool m_Wireframe = false;
	void SetRasterState();

	// Model data
	std::vector<Mesh*> m_Meshes;

	std::string m_Name;
	std::string texture_diffuse;
	std::string texture_normal;

	bool m_IsLoaded = false;
	bool m_UseDiffuseTexture = false;
	bool m_UseNormalTexture = false;
};