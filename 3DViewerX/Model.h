#pragma once

#include <d3d11_4.h>
#include <DirectXMath.h>
#include "Renderer.h"
#include <vector>
#include "WindowEvents.h"
#include "InputEvents.h"
#include <string>
#include "Mesh.h"

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

	//buffer data
	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11Buffer* m_IndexBuffer = nullptr;

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

	bool m_IsLoaded = false;
	bool m_UseDiffuseTexture = false;
	bool m_UseNormalTexture = false;
};